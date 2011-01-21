/**
 *  \file       theLink/libmsgque/uds_io.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef UDS_IO_H
#   define UDS_IO_H

#   define UDS_SAVE_ERROR(uds) ((uds)?IO_SAVE_ERROR((uds)->io):NULL)

#if defined(MQ_IS_POSIX)

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                  uds_io                                   */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE UdsCreate (
  struct MqIoS * const io,
  struct UdsS ** const udsPtr
);

enum MqErrorE UdsDelete (
  struct UdsS ** const udsP
) __attribute__((nonnull));

enum MqErrorE UdsServer (
  register struct UdsS * const uds
);

enum MqErrorE UdsConnect (
  register struct UdsS * const uds
);

/*****************************************************************************/
/*                                                                           */
/*                                uds_arg                                    */
/*                                                                           */
/*****************************************************************************/

END_C_DECLS

#else /* MQ_IS_POSIX */

#define UdsCreate(io,udsPtr)
#define UdsDelete(udsP) 
#define UdsServer(uds)
#define UdsConnect(uds)

#endif /* MQ_IS_POSIX */

#endif /* UDS_IO_H */


