/**
 *  \file       theLink/libmsgque/generic_io.c
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
#include "sys_com.h"

#define MQ_CONTEXT_S  generiC->context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                 init                                      */
/*                                                                           */
/*****************************************************************************/

void GenericCreate (void) {
#if defined(_MSC_VER)
  SysWSAStartup (NULL);
#endif
}

void GenericDelete (void) {
#if defined(_MSC_VER)
  SysWSACleanup (NULL);
#endif
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

/// \attention only the PARENT is using #GenericCreate
enum MqErrorE
pGenericCreate (
  struct MqIoS * const io,
  struct GenericS ** const out
)
{
  struct MqS * const context = io->context;

  // ok we have an TCP style communication
  register struct GenericS * const generiC = *out ? *out :
    (*out = (struct GenericS *const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*generiC)));

  generiC->io = io;
  generiC->context = context;
  generiC->sock = -1;

  return MQ_OK;
}

void
pGenericDelete (
  struct GenericS ** const genericP
)
{
  if (unlikely(genericP == NULL || *genericP == NULL)) {
    return;
  } else {
    struct GenericS * const generiC = *genericP;
    struct MqS * const context = generiC->context;
    SysCloseSocket (context, __func__, MQ_YES, &generiC->sock);
    MqSysFree (*genericP);
  }
}

/// \brief create a socket for \e TCP or \e UDS
enum MqErrorE
GenericCreateSocket (
  struct GenericS * const generiC,	///< [in] generic local storage
  int const domain,			///< [in] socket domain (should be: AF_INET or AF_UNIX)
  int const type,			///< [in] socket type (should be: SOCK_STREAM)
  int protocol				///< [in] the socket protocol (should be: 0)
)
{
  // create socket
  MqErrorCheck (SysSocket (MQ_CONTEXT_S, domain, type, protocol, &generiC->sock));

//printLI(generiC->sock)

error:
  return MqErrorStack (MQ_CONTEXT_S);
}

enum MqErrorE
GenericServer (
  struct GenericS * const generiC,	///< the current generic handle
  struct sockaddr * const sockaddr,	///< the address package
  socklen_t const sockaddrlen		///< the length of the address
) {
  struct MqS * const context = MQ_CONTEXT_S;
  MQ_SOCK child_sock;
  struct sockaddr mysockaddr; 
  socklen_t mysockaddrlen;
  // this is just a flag
  struct MqIdS id;
  //const int serverStartup = (context->config.startAs != MQ_START_DEFAULT);

  // the parent server should "forget" his child's
  // server / child dependency management is done by libmsgque
  if (	    context->config.startAs == MQ_START_SPAWN
#if defined(HAVE_FORK)
	||  context->config.startAs == MQ_START_FORK
#endif
     ) {
    // do not create a "defunc" process
    MqErrorCheck (MqSysIgnorSIGCHLD(context));
  }

  // 1. create socket
  MqErrorCheck (GenericBind (generiC, sockaddr, sockaddrlen));

  // 2. listen on port
  MqErrorCheck (SysListen (context, generiC->sock, 128));

  // 2a. add listener socket to the select list
/*
  if (serverStartup) {
    pEventAdd(__func__, context, &generiC->sock);
    // allow the call to the 'Event' proc
    context->link.bits.onCreateEnd = MQ_YES;
  }
*/

  // start the server required
  do {
    id.type = MQ_ID_UNUSED;
    mysockaddrlen = sizeof(mysockaddr);
    // wait for a connection request and process events
/*
    if (serverStartup) {
      if (MqErrorCheckI(pWaitOnEvent (context, MQ_SELECT_RECV, LONG_MAX))) {
	return pErrorSetExitWithCheck(context);
      }
    }
*/
    // 3. accept incomming call, on error shutdown server
    if (MqErrorCheckI(SysAccept (context, generiC->sock, &mysockaddr, &mysockaddrlen, &child_sock)))
      return pErrorSetExitWithCheck(context);
    // select: how to start the new task
    switch (context->config.startAs) {
      case MQ_START_FORK: {
#if defined(HAVE_FORK)
	MqErrorCheck (pIoStartServer(context->link.io,MQ_START_SERVER_AS_INLINE_FORK, &child_sock, &id));
#endif
	break;
      }
      case MQ_START_THREAD: {
#if defined(MQ_HAS_THREAD)
	MqErrorCheck (pIoStartServer(context->link.io,MQ_START_SERVER_AS_THREAD, &child_sock, &id));
	// keep the "child_sock" open, because it's still the same process
#endif
	break;
      }
      case MQ_START_SPAWN: {
	MqErrorCheck (pIoStartServer(context->link.io,MQ_START_SERVER_AS_SPAWN, &child_sock, &id));
	break;
      }
      case MQ_START_DEFAULT: {
	// server setup without "--fork,--thread or --spawn", this is just a singel request server.
	// The server will shutdown on the end of the link.
	break;
      }
    }

  // for a NON --fork/--thread/--spawn server the 'id.val' will allways be '0' -> continue
  // for a --fork/--thread/--spawn parent the 'id.val' will be set by pIoStartServer to something else 
  //   than 0 -> loop
  // for a --fork/--thread/--spawn child the 'id.val' will be '0' -> continue
  } while (
    id.type != MQ_ID_UNUSED && id.val != 0UL
  );
  
  // \attention only the inline fork child (MQ_START_FORK) or the non --fork/--thread/--spawn server 
  // (MQ_START_DEFAULT) will go behind this line

  // close listener socket
  MqErrorCheck (SysCloseSocket (context, __func__, MQ_NO, &generiC->sock));

  // delete listener socket from select list
/*
  if (serverStartup) {
    pEventDel(context);
    // disallow the call to the 'Event' proc
    context->link.bits.onCreateEnd = MQ_NO;
  }
*/

  // child socket is now the main socket
  generiC->sock = child_sock;

  // 3. add child socket to the select list and configure the new socket
  pIoEventAdd(__func__, generiC->io, &generiC->sock);

  // 4. the fork-server should "allow" the SIGCHLD
  if (	    context->config.startAs == MQ_START_SPAWN
#if defined(HAVE_FORK)
	||  context->config.startAs == MQ_START_FORK
#endif
     ) {
    /*
     * TCL complain if SIGCHLD is ignored 
     * -> additional information from TCL source-code:
     * This changeup in message suggested by Mark Diekhans
     * to remind people that ECHILD errors can occur on
     * some systems if SIGCHLD isn't in its default state.
     */
    MqErrorCheck (MqSysAllowSIGCHLD(context));
  }

  // 5, finish
error:
  return MqErrorStack (context);
}

enum MqErrorE
GenericConnect (
  struct GenericS * const generiC,	    ///< [in] the current generic handle
  struct sockaddr * const sockaddr,	    ///< [in] the address package
  socklen_t const sockaddrlen,		    ///< [in] the length of the address
  MQ_TIME_T timeout			    ///< [in] timeout
)
{
  struct MqS * const context = MQ_CONTEXT_S;
  //int status = 1;

  // 1. config socket
  MqErrorCheck (SysSetAsync (context, generiC->sock));

  // 2. reuse socket after socket was closed
/*
  MqErrorCheck (
    SysSetSockOpt(context, generiC->sock, SOL_SOCKET, SO_REUSEADDR, (MQ_sockopt_optval_T) &status, sizeof (status))
  );
*/

  // 2. connect to server
  MqErrorCheck (SysConnect (context, generiC->sock, sockaddr, sockaddrlen, timeout));

  // 3. add Event to Listen on
  pIoEventAdd(__func__, generiC->io, &generiC->sock);

error:
  return MqErrorStack (context);
}

enum MqErrorE
GenericBind (
  struct GenericS const * const generiC,    ///< the current generic handle
  struct sockaddr *sockaddr,		    ///< the address package
  socklen_t sockaddrlen 		    ///< the length of the address
)
{
  struct MqS * const context = MQ_CONTEXT_S;
  int status = 1;
  MqErrorCheck (SysSetSockOpt
                (context, generiC->sock, SOL_SOCKET, SO_REUSEADDR,
                 (MQ_sockopt_optval_T) &status, sizeof (status)));
  MqErrorCheck (SysBind (context, generiC->sock, sockaddr, sockaddrlen));

error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                                set/get                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
GenericGetSockName (
  struct GenericS const * const generiC,    ///< the current generic handle
  struct sockaddr *sockaddr,		    ///< the address package
  socklen_t *sockaddrlen 		    ///< the length of the address
)
{
  return SysGetSockName (MQ_CONTEXT_S, generiC->sock, sockaddr, sockaddrlen);
}

END_C_DECLS







