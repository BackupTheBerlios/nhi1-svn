/**
 *  \file       theLink/libmsgque/pipe_io.c
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
#include "pipe_io.h"
#include "io_private.h"
#include "error.h"
#include "sys_com.h"

/*###########################################################################*/
/*###                                                                     ###*/
/*###                              posix                                  ###*/
/*###                                                                     ###*/
/*###########################################################################*/

#   include "generic_io.h"
#   include "log.h"
#   include "sys.h"
#   include "buffer.h"
#   include "bufferL.h"

#   define MQ_CONTEXT_S  pipe->io->context

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

/** -brief ...
 */
struct PipeS {
  struct MqIoS * io;		    ///< link to 'io' object
  struct GenericS * generiC;	    ///< generic part of the connection

  struct MqIoPipeConfigS *config;   ///< to to global configuration data
};

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
PipeCreate (
  struct MqIoS * const io,
  struct PipeS ** const pipePtr
)
{
  struct MqS * const context = io->context;

  // ok we have an PIPE style communication
  register struct PipeS * const pipe = *pipePtr ? *pipePtr : 
    (*pipePtr = (struct PipeS *const) MqSysCalloc (MQ_ERROR_PANIC, 1, sizeof (*pipe)));

  // init
  pipe->io = io;
  pipe->config = &io->context->config.io.pipe;

  if (MQ_IS_CLIENT (context)) {
    // create the socketpair
    MqErrorCheck (SysSocketPair (context, pipe->config->socket));
    MqDLogV(context,4,"create SOCKETPAIR -> sock[0]<%i>, sock[1]<%i>\n", pipe->config->socket[0], pipe->config->socket[1]);
  } else {
    if (pipe->config->socket[1] < 0)
      return MqErrorDb (MQ_ERROR_NOT_A_CLIENT);
    // close the "client" socket of the socketpair -> who set socket[0] ?
    //MqErrorCheck(SysCloseSocket(context,MQ_NO,&pipe->config->socket[0]));
    MqDLogV(context,4,"get SOCKET -> %i\n", pipe->config->socket[1]);
  }

error:
  return MqErrorStack (context);
}

void PipeDelete (
  struct PipeS ** const pipeP
)
{
  struct PipeS * pipe = *pipeP;
  if (unlikely(pipe == NULL)) return;
  SysCloseSocket (MQ_CONTEXT_S, __func__, MQ_YES, (MQ_IS_CLIENT(MQ_CONTEXT_S) ?  &pipe->config->socket[0] : &pipe->config->socket[1]));
  MqSysFree(*pipeP);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE
PipeServer (
  struct PipeS * const pipe
)
{
  // listen for events on the "server" socket of the socketpair
  return pIoEventAdd(pipe->io, &pipe->config->socket[1]);
}

enum MqErrorE
PipeConnect (
  struct PipeS * const pipe
)
{
  // listen for event on the "client" socket of the socketpair
  return pIoEventAdd(pipe->io, &pipe->config->socket[0]);
}

/*****************************************************************************/
/*                                                                           */
/*                                set/get                                    */
/*                                                                           */
/*****************************************************************************/

MQ_PTR
PipeGetOpt (
  struct PipeS const * const pipe,
  int const opt
) {
  if (unlikely (pipe == NULL))
    return NULL;
  return NULL;
}

MQ_SOCK*
PipeGetServerSocket (
  struct PipeS * const pipe
) {
  return &pipe->config->socket[1];
}

END_C_DECLS


