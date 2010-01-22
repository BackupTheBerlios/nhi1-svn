/**
 *  \file       theLink/libmsgque/mq_io.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_IO_H
#   define MQ_IO_H

/*****************************************************************************/
/*                                                                           */
/*                                io_init                                    */
/*                                                                           */
/*****************************************************************************/

BEGIN_C_DECLS

/// \brief everything \e io need for local storage
struct MqIoS {
  struct MqS * context;             ///< link to the 'msgque' object
  union {
#if defined(MQ_IS_POSIX)
    struct UdsS *   udsSP;          ///< UDS object pointer
#endif
    struct TcpS *   tcpSP;          ///< TCP object pointer
    struct PipeS *  pipeSP;         ///< PIPE object pointer
  } iocom;                          ///< link to 'iocom' object
  fd_set fdset;                     ///< select need this
  MQ_SOCK *sockP;                   ///< pointer to the current socket
  struct MqIdS id;                  ///< (pid_t/mqthread_t) id of the server in "pipe" mode
  struct MqEventS * event;          ///< link to the according Event structure
  struct MqIoConfigS * config;      ///< "Io" part of the global configuration data
};

enum MqErrorE pIoCreate (
  struct MqS * const context,
  struct MqIoS ** const out
) __attribute__((nonnull));

void pIoDelete (
  struct MqIoS ** const ioP
) __attribute__((nonnull));

void pIoShutdown (
  struct MqIoS * const io
);

/*****************************************************************************/
/*                                                                           */
/*                                 io_atom                                   */
/*                                                                           */
/*****************************************************************************/

void pIoCloseSocket (
  struct MqIoS * const io,
  MQ_CST const caller
);

enum MqErrorE pIoRead (
  struct MqIoS const * const io,
  register struct MqBufferS * const buf,
  MQ_SIZE const numBytes
);

enum MqErrorE pIoSend (
  struct MqIoS const * const io,
  struct MqBufferS const * const buf
);

enum MqErrorE pIoConnect (
  struct MqIoS * const io
);

enum MqErrorE pIoSelect (
  struct MqIoS * const io,
  enum MqIoSelectE const typ,
  struct timeval * const timeout
);

enum MqErrorE pIoSelectAll (
  struct MqIoS * const io,
  enum MqIoSelectE const typ,
  struct timeval * const timeout
);

enum MqErrorE pIoSelectStart (
  struct MqIoS * const io,
  struct timeval const * const timeout,
  EventCreateF const proc
);

/*****************************************************************************/
/*                                                                           */
/*                                 io_misc                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief howto start a new server
enum IoStartServerE {
  MQ_START_SERVER_AS_PIPE,
#if defined(HAVE_FORK)
  MQ_START_SERVER_AS_FORK,
  MQ_START_SERVER_AS_INLINE_FORK,
#endif
#if defined(MQ_HAS_THREAD)
  MQ_START_SERVER_AS_THREAD,
#endif
  MQ_START_SERVER_AS_SPAWN,
};

/// \brief start the PIPE Server 
///
/// with \e fMain and \e name we have the possibility to select
/// howto create a process
enum MqErrorE pIoStartServer (
  struct MqIoS * const io,
  enum IoStartServerE startType,
  MQ_SOCK * sockP,
  struct MqIdS * idP
);

/// \brief helper to convert #IoStartServerE into a human readable string
MQ_CST pIoLogStartType (
  enum IoStartServerE startType
);

MQ_STR pIoLogId (
  MQ_STR buf,
  MQ_INT size,
  struct MqIdS id
);

/// \brief helper to covert the #MqIoComE into a human readable string
MQ_CST pIoLogCom (
  enum MqIoComE com
);

enum MqErrorE pIoEventAdd (
  struct MqIoS * const io,
  MQ_SOCK * sockP
);

struct MqS const * pIoGetMsgqueFromSocket (
  struct MqIoS * const	io,
  MQ_SOCK sock
);

MQ_BOL pIoIsRemote (struct MqIoS * const);

static mq_inline MQ_BOL pIoCheck (
  struct MqIoS * const io
) {
  return (io && io->sockP ? (*(io->sockP) >= 0) : MQ_NO);
};

static mq_inline MQ_TIME_T pIoGetTimeout (
  struct MqIoS * const io
)
{
  return io->config->timeout;
};

#if _DEBUG
void
pIoLog (
  struct MqIoS * const  io,
  MQ_CST proc
);
#endif

END_C_DECLS

#endif /* MQ_IO_H */



