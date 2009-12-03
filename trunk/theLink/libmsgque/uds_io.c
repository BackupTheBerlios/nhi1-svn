/**
 *  \file       theLink/libmsgque/uds_io.c
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

#if defined(MQ_IS_POSIX)

#include "uds_io.h"
#include "io_private.h"
#include "error.h"

/*###########################################################################*/
/*###                                                                     ###*/
/*###                              posix                                  ###*/
/*###                                                                     ###*/
/*###########################################################################*/

#include "generic_io.h"
#include "log.h"
#include "sys.h"
#include "buffer.h"
#include "bufferL.h"

#include <sys/un.h>

#define MQ_CONTEXT_S  uds->io->context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

/** -brief ...
 */
struct UdsS {
  struct MqIoS const * io;	  ///< link to 'io' object
  struct GenericS * generic;	  ///< generic part of the connection
  struct sockaddr_un sockaddr;	  ///< socket address
  struct MqIoUdsConfigS *config;  ///< file-name
};

static void
sUdsFillAddress (
  struct UdsS * const uds
)
{
  strcpy (uds->sockaddr.sun_path, uds->config->file->cur.C);
  uds->sockaddr.sun_family = AF_UNIX;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
UdsCreate (
  struct MqIoS * const io,
  struct UdsS ** const udsPtr
)
{
  struct MqS * const context = io->context;

  // ok we have an UDS style communication
  register struct UdsS * const uds = *udsPtr =(struct UdsS *) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*uds));

  // init uds data
  uds->io   = io;
  uds->config = &io->context->config.io.uds;

  // check UDS options
  if (uds->config->file == NULL || uds->config->file->cursize == 0) {
    return MqErrorDbV2(context, MQ_ERROR_OPTION_REQUIRED, "UDS", "--file");
  }

  // intit the generic part
  MqErrorCheck (GenericCreate (io, &uds->generic));

  // set the socket
  MqErrorCheck (GenericCreateSocket (uds->generic, AF_UNIX, SOCK_STREAM, 0));

  sUdsFillAddress (uds);

error:
  return MqErrorStack (context);
}

enum MqErrorE
UdsDelete (
  struct UdsS ** const udsP
)
{
  struct UdsS * const uds = *udsP;
  if (unlikely(!uds)) return MQ_OK;   // nothing to do

  GenericDelete (&uds->generic);
  MqBufferDelete (&uds->config->file);
  MqSysFree (*udsP);

  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
UdsServer (
  register struct UdsS * const uds
)
{
  struct MqS * const context = MQ_CONTEXT_S;

  // delete the UDS file (only if it exists)
  MqErrorCheck (SysUnlink(context,uds->config->file->cur.C));

  MqDLogV(context, 2, "create UNIX uds socket file<%s>\n", uds->config->file->cur.C);
  MqErrorCheck (GenericServer
                (uds->generic, (struct sockaddr *) &uds->sockaddr, sizeof (struct sockaddr_un)));

  // if it is the !only! server context then cleanup the uds socket file 
  if (context->config.startAs == MQ_START_DEFAULT) {
    MqDLogV(context, 2, "delete UNIX uds socket file<%s>\n", uds->config->file->cur.C);
    MqErrorCheck (SysUnlink (context, uds->config->file->cur.C));
  }

error:
  return MqErrorStack (context);
}

enum MqErrorE
UdsConnect (
  register struct UdsS * const uds
)
{
  struct MqS * const context = MQ_CONTEXT_S;

  // do the connect
  MqErrorCheck (GenericConnect (uds->generic, (struct sockaddr *const ) &uds->sockaddr, 
		    sizeof (struct sockaddr_un), uds->io->config->timeout));

/**
 * now get sockaddr !after! successful connect
 * calling GenericGetSockName on UNIX domain sockets is NOT portable
 * BSD style UNIX's does not fully support this
 *
 *   MqErrorCheck (GenericGetSockName
 *              (uds->generic, (struct sockaddr *) &uds->sockaddr, sockaddrlen));
 */

  // finaly get uds->config->file
  MqBufferSetC (uds->config->file, uds->sockaddr.sun_path);

  MqDLogV (context, 1, "local socket file<%s>\n", (MQ_STR) uds->config->file->data);

error:
  return MqErrorStack (context);
}

/*****************************************************************************/
/*                                                                           */
/*                                set/get                                    */
/*                                                                           */
/*****************************************************************************/

END_C_DECLS

#endif




