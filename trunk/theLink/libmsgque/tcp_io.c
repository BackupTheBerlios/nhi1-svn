/**
 *  \file       theLink/libmsgque/tcp_io.c
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "main.h"
#include "tcp_io.h"
#include "generic_io.h"
#include "mq_io.h"
#include "error.h"
#include "log.h"
#include "sys.h"
#include "buffer.h"
#include "bufferL.h"

#define MQ_CONTEXT_S tcp->io->context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

/** -brief everything needed for a \e Tcp context link
 */
struct TcpS {

  struct MqIoS const * io;	    ///< link to 'io' object
  struct GenericS * generiC;	    ///< generic part of the connection

  struct addrinfo * remoteaddr;	    ///< remote address-info
  struct addrinfo * localaddr;	    ///< local address-info

  struct MqIoTcpConfigS * config;   ///< configuration data
};

static enum MqErrorE
sTcpCreateAddrInfo (
  struct TcpS const * const tcp,    ///< [in] the tcp object in duty
  struct MqBufferS * const host,    ///< [in] the host name
  struct MqBufferS * const port,    ///< [in] the port name
  struct addrinfo ** adressinfo	    ///< [out] the linked list of addrinfo
)
{
  struct addrinfo hints;
  MQ_CST hoststr = NULL , portstr = NULL;

  // 1. get the host (NULL if not set)
  if (host && host->cursize) {
    MqErrorCheck(MqBufferGetC (host, &hoststr));
  }

  // 2. get the port (NULL if not set)
  if (port && port->cursize) {
    MqErrorCheck(MqBufferGetC (port, &portstr));
  }

  // 3. fill the hints
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  // 4. Call the getaddrinfo
  MqErrorCheck(SysGetAddrInfo(MQ_CONTEXT_S,hoststr,portstr,&hints,adressinfo));

error:
  return MqErrorStack (MQ_CONTEXT_S);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
TcpCreate (
  struct MqIoS * const io,
  struct TcpS ** const tcpPtr
)
{
  struct MqS * const context = io->context;
  MQ_CST host;
  MQ_INT port;

  // ok we have an TCP style communication
  register struct TcpS * const tcp = *tcpPtr ? *tcpPtr : 
    (*tcpPtr = (struct TcpS * const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*tcp)));

  // init
  tcp->io = io;
  tcp->config = &io->context->config.io.tcp;

  // validate
  if (tcp->config->host == NULL) {
    tcp->config->host = MQ_IS_SERVER(io->context) ?
      // the SERVER listen by default to ALL interfaces
      MqBufferCreateC (MQ_ERROR_PANIC, "0.0.0.0") :
	// the CLIENT connect by default to localhost
	MqBufferCreateC (MQ_ERROR_PANIC, "localhost");
  }

  if (tcp->config->port == NULL)
    return MqErrorDbV2(context, MQ_ERROR_OPTION_REQUIRED,"TCP","--port");

  // init the generic part
  MqErrorCheck (pGenericCreate (io, &tcp->generiC));

  // fill remote address
  if (tcp->remoteaddr == NULL)
    MqErrorCheck (sTcpCreateAddrInfo (tcp, tcp->config->host, tcp->config->port, &tcp->remoteaddr));

  // fill my address
  if (tcp->config->myhost || tcp->config->myport) {
    if (MQ_IS_CLIENT (context)) {
      if (tcp->localaddr == NULL)
	MqErrorCheck (sTcpCreateAddrInfo (tcp, tcp->config->myhost, tcp->config->myport, &tcp->localaddr));
    } else if(tcp->config->myhost) {
      return MqErrorDbV2 (context, MQ_ERROR_OPTION_WRONG,"server","--myhost");
    } else if(tcp->config->myport) {
      return MqErrorDbV2 (context, MQ_ERROR_OPTION_WRONG,"server","--myport");
    }
  }

  // set the socket
  MqErrorCheck (GenericCreateSocket (tcp->generiC, tcp->remoteaddr->ai_family, 
		    tcp->remoteaddr->ai_socktype, tcp->remoteaddr->ai_protocol));

  // get socket data
  MqErrorCheck(SysGetTcpInfo(context,(struct sockaddr_in*)tcp->remoteaddr->ai_addr,&host,&port));
  MqDLogV(context, 4, "%s socket host<%s> and port<%u>\n", (MQ_IS_SERVER(context)?"local":"remote"),host, port);

error:
  return MqErrorStack (context);
}

void 
TcpDelete (
  struct TcpS ** const tcpP
)
{
  struct TcpS * const tcp = *tcpP;
  if (unlikely(!tcp))	return;		// nothing to do

  pGenericDelete (&tcp->generiC);

  MqSysFreeAddrInfo(&tcp->remoteaddr);
  MqSysFreeAddrInfo(&tcp->localaddr);

  MqSysFree (*tcpP);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
TcpServer (
  register struct TcpS * const tcp
)
{
  GenericServer (tcp->generiC, tcp->remoteaddr->ai_addr, tcp->remoteaddr->ai_addrlen);
  return MqErrorStack (MQ_CONTEXT_S);
}

enum MqErrorE
TcpConnect (
  register struct TcpS * const tcp
) {
  struct MqS * const context = MQ_CONTEXT_S;
  struct sockaddr_in  addr;
  socklen_t	      addrLen;

  // create my socket adress
  if (tcp->localaddr) {
    MqErrorCheck (GenericBind (tcp->generiC, tcp->localaddr->ai_addr, tcp->localaddr->ai_addrlen));
  }

  // do the connect
  MqErrorCheck (GenericConnect
    (tcp->generiC, tcp->remoteaddr->ai_addr, tcp->remoteaddr->ai_addrlen, tcp->io->config->timeout));

  // get the setup !after! connection
  addrLen = sizeof(addr);
  MqErrorCheck (GenericGetSockName (tcp->generiC, (struct sockaddr*) &addr, &addrLen));

  // finaly get tcp->config->host and tcp->config->port
  if (unlikely(context->config.debug >= 4)) {
    MQ_CST host;
    MQ_INT port;
    MqErrorCheck(SysGetTcpInfo(context,&addr,&host,&port));
    MqLogV(context, __func__, 4, "local socket host<%s> and port<%u>\n", host, port);
  }

error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                                debugging                                  */
/*                                                                           */
/*****************************************************************************/

#ifdef _DEBUG
void
TcpLogAddr (
  struct MqS * const context,
  struct addrinfo * addrinfo
)
{
  struct sockaddr_in * sockaddr_in = (struct sockaddr_in *) addrinfo->ai_addr;
  MQ_CST str;
  MQ_INT port;
  MqLogV (context, __func__, 0, ">>>> sockaddr_in (%p)\n", (void*) sockaddr_in);
  str = (sockaddr_in->sin_family == AF_INET ? "AF_INET" : "WRONG!!");
  MqLogV (context, __func__, 0, " sin_family = %s(%i)\n", str, sockaddr_in->sin_family);
  SysGetTcpInfo(context,sockaddr_in,&str,&port);
  MqLogV (context, __func__, 0, " sin_port   = %u\n", port);
  MqLogV (context, __func__, 0, " sin_addr   = %s\n", str);
  MqLogV (context, __func__, 0, "<<<< sockaddr_in\n");
}
#endif

END_C_DECLS







