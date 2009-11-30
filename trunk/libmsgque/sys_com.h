/**
 *  \file       libmsgque/sys_com.h
 *  \brief      \$Id: sys_com.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef SYS_COM_H
#define SYS_COM_H

#include "error.h"
#include <fcntl.h>

#if defined(MQ_IS_POSIX)
#   include <sys/socket.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#elif defined(MQ_IS_WIN32)
//  we require WindowsXP or higher -> used for "getaddrinfo"
#		if !defined(_MSC_VER)
#			define WINVER WindowsXP
#			include <w32api.h>
#		endif
#   include <winsock2.h>
#   include <Ws2tcpip.h>
#endif

struct sockaddr;

BEGIN_C_DECLS

#ifdef MQ_IS_WIN32
# define MQ_sockopt_optval_T char*
# define MQ_buf_T char*
#else
# define MQ_sockopt_optval_T void*
# define MQ_buf_T void*
#endif


/*****************************************************************************/
/*                                                                           */
/*                                sys_com                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE SysAccept (
  struct MqS * const context,
  MQ_SOCK server,
  struct sockaddr *addr,
  socklen_t * addrlen,
  MQ_SOCK * client
);

enum MqErrorE SysGetSockOpt (
  struct MqS * const context,
  MQ_SOCK const socket,
  int const level,
  int const optname,
  MQ_sockopt_optval_T optval,
  socklen_t * const optlen
);

enum MqErrorE SysSetSockOpt (
  struct MqS * const context,
  MQ_SOCK const socket,
  int const level,
  int const optname,
  const MQ_sockopt_optval_T const optval,
  socklen_t const optlen
);

// enum MqErrorE
// SysSetSockDebug(struct MqS *  const context, MQ_SOCK socket);

enum MqErrorE SysConnect (
  struct MqS * const context,
  MQ_SOCK const socket,
  struct sockaddr * const serv_addr,
  socklen_t const addrlen,
  MQ_TIME_T const timeout
);

enum MqErrorE SysSocket (
  struct MqS * const context,
  int domain,
  int type,
  int protocol,
  MQ_SOCK * sock
);

enum MqErrorE SysSocketPair (
  struct MqS * const context,
  int socks[]
);

enum MqErrorE SysSetAsync (
  struct MqS * const context,
  MQ_SOCK socket
);

#if defined(MQ_IS_WIN32)

enum MqErrorE SysWSAStartup (
  struct MqS * const context
);

enum MqErrorE SysWSACleanup (
  struct MqS * const context
);

#endif

enum MqErrorE SysBind (
  struct MqS * const context,
  MQ_SOCK socket,
  const struct sockaddr *my_addr,
  const socklen_t addrlen
);

enum MqErrorE SysSelect (
  struct MqS * const context,
  int n,
  fd_set * readfds,
  fd_set * writefds,
  struct timeval const * const timeout
);

enum MqErrorE SysListen (
  struct MqS * const context,
  MQ_SOCK socket,
  int backlog
);

enum MqErrorE SysGetSockName (
  struct MqS * const context,
  MQ_SOCK socket,
  struct sockaddr *sockaddr,
  socklen_t * sockaddrlen
);

/*
enum MqErrorE SysShutdown (
  struct MqS * const context,
  MQ_SOCK hdl
);
*/

enum MqErrorE SysCloseSocket (
  struct MqS * const context,
  MQ_CST const caller,
  MQ_BOL doShutdown,
  MQ_SOCK * hdl
);

enum MqErrorE SysSend (
  struct MqS * const context,    
  MQ_SOCK hdl,			    
  MQ_BIN buf,			    
  MQ_SIZE numBytes,		    
  int flags,			    
  MQ_TIME_T timeout		    
);

enum MqErrorE SysRecv (
  struct MqS * const context,    
  MQ_SOCK const hdl,		    
  MQ_BIN buf,			    
  MQ_SIZE numBytes,		    
  MQ_SIZE * const newSize,		    
  MQ_TIME_T timeout		    
);

/*****************************************************************************/
/*                                                                           */
/*                            address information                            */
/*                                                                           */
/*****************************************************************************/

void
SysFreeAddrInfo(
    struct addrinfo **res
);

enum MqErrorE
SysGetAddrInfo(
    struct MqS * const context,
    MQ_CST node,
    MQ_CST service,
    const struct addrinfo *hints,
    struct addrinfo **res
);

enum MqErrorE
SysGetTcpInfo(
  struct MqS * const context,
  struct sockaddr_in * addr,
  MQ_CST *host,
  MQ_INT *port	
);

END_C_DECLS

#endif /* SYS_COM_H */


