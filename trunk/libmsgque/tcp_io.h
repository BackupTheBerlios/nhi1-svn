/**
 *  \file       libmsgque/tcp_io.h
 *  \brief      \$Id: tcp_io.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef TCP_IO_H
#   define TCP_IO_H

#   define TCP_SAVE_ERROR(tcp) ((tcp)?IO_SAVE_ERROR((tcp)->io):NULL)

struct sockaddr_in;

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                 tcp_io                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE TcpCreate (
  struct MqIoS * const io,
  struct TcpS ** const tcpPtr
);

void TcpDelete (
  struct TcpS ** const tcpP
) __attribute__((nonnull));

enum MqErrorE TcpServer (
  register struct TcpS * const tcp
);

enum MqErrorE TcpConnect (
  register struct TcpS * const tcp
);

END_C_DECLS

#endif /* TCP_IO_H */



