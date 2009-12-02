/**
 *  \file       libmsgque/io_private.h
 *  \brief      \$Id: io_private.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef IO_PRIVATE_H
#   define IO_PRIVATE_H

#   define IO_SAVE_ERROR(io) ((io)?MQ_SAVE_ERROR((io)->link):NULL)

#   include "mq_io.h"
#   include "sys_com.h"

BEGIN_C_DECLS

/// \brief everything \e io need for local storage
struct MqIoS {
  struct MqS * context;		    ///< link to the 'msgque' object
  union {
#if defined(MQ_IS_POSIX)
    struct UdsS *   udsSP;	    ///< UDS object pointer
#endif
    struct TcpS *   tcpSP;	    ///< TCP object pointer
    struct PipeS *  pipeSP;	    ///< PIPE object pointer
  } iocom;			    ///< link to 'iocom' object
  fd_set fdset;			    ///< select need this
  MQ_SOCK *sockP;		    ///< pointer to the current socket
  struct MqIdS id;		    ///< (pid_t/mqthread_t) id of the server in "pipe" mode
  struct MqEventS * event;	    ///< link to the according Event structure
  struct MqIoConfigS * config;	    ///< "Io" part of the global configuration data
};

END_C_DECLS

#endif /* IO_PRIVATE_H */


