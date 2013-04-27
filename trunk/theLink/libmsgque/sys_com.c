/**
 *  \file       theLink/libmsgque/sys_com.c
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

#if defined(MQ_IS_POSIX)
#   define SHUTDOWN_BOTH SHUT_RDWR
#   define SOCKET_ERROR -1
#   define INVALID_SOCKET -1
#   define WIN32_WSA(x)   x
#   define WIN32_socket(x)   x
//#   if defined(SOLARIS_I386)
//#      define BSD_COMP
//#   endif /* SOLARIS_I386 */
#   include <sys/ioctl.h>
#   if HAVE_SYS_FILIO_H
#     include <sys/filio.h>
#   endif
#   include <errno.h>
#   include <signal.h>

#elif defined(MQ_IS_WIN32)
#   define SHUTDOWN_BOTH SD_BOTH
#   define WIN32_WSA(x)   WSA##x
#   define WIN32_socket(x)   x##socket
#   define WSAENOENT 9999
#else /* MQ_IS_WIN32 */
#   error not supported
#endif /* MQ_IS_WIN32 */

#define MQ_CONTEXT_S context

BEGIN_C_DECLS

void SysComCreate (void) 
{
#if defined(MQ_IS_POSIX)
  signal(SIGPIPE, SIG_IGN);
#endif
  MqLal.SysSelect = (MqSysSelectF) select;
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_private                                */
/*                                                                           */
/*****************************************************************************/

static enum MqErrorE
sSysMqErrorMsg (
  struct MqS * const context,
  MQ_CST prefix,
  MQ_CST  text
)
{
  char buffer[256];
  int num;
  char *str;

  if (context == MQ_ERROR_IGNORE) return MQ_ERROR;

  mq_snprintf(buffer,256,MqMessageText[MQ_ERROR_CAN_NOT],text);
#if defined(MQ_IS_POSIX)
  num = errno;
  str = strerror (num);
#elif defined(MQ_IS_WIN32)
  num = WSAGetLastError ();
  str = strerror (num);
#else
#   error not supported
#endif
  return MqErrorV (context, prefix, num, "%s -> ERRNO<%i> ERR<%s>", buffer, num, str);
}

#if defined(MQ_IS_POSIX)
#   define sSysGetErrorNum errno
#   define sSysSetErrorNum(i) errno=i
#elif defined(MQ_IS_WIN32)
#   define sSysGetErrorNum WSAGetLastError()
#   define sSysSetErrorNum(i) WSASetLastError(i)
#endif

/*****************************************************************************/
/*                                                                           */
/*                                sys_socket                                 */
/*                                                                           */
/*****************************************************************************/

#ifdef MQ_IS_WIN32

enum MqErrorE
SysWSAStartup (
  struct MqS * const context
)
{
  WORD wVersionRequested;
  WSADATA wsaData;
  int err, max = 2, min = 2;

  wVersionRequested = MAKEWORD (max, min);

  err = WSAStartup (wVersionRequested, &wsaData);

  if (err != 0) {
    return MqErrorDbV (MQ_ERROR_WINSOCK, "WSAStartup", err);
  }

  /* Confirm that the WinSock DLL supports 2.2. */
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */

  if (LOBYTE (wsaData.wVersion) != max || HIBYTE (wsaData.wVersion) != min) {
    WSACleanup ();
    return MqErrorDbV (MQ_ERROR_WINSOCK, "WSAStartup", WSAGetLastError());
  }

  /* The WinSock DLL is acceptable. Proceed. */
  return MQ_OK;
}

enum MqErrorE
SysWSACleanup (
  struct MqS * const context
)
{
  int err;
  if ((err = WSACleanup ()) != 0) {
    return MqErrorDbV (MQ_ERROR_WINSOCK, "WSACleanup", WSAGetLastError());
  }
  return MQ_OK;
}

#endif /* MQ_IS_WIN32 */


enum MqErrorE
SysAccept (
     struct MqS * const context,     
     MQ_SOCK server,            
     struct sockaddr *addr,     
     socklen_t *addrlen,       
     MQ_SOCK *client            
) {
next:
  if (unlikely ((*client = (MQ_SOCK) accept (server, addr, addrlen)) == INVALID_SOCKET)) {
    if (sSysGetErrorNum == WIN32_WSA (EWOULDBLOCK))
      goto next;
    return sSysMqErrorMsg (context, __func__, "accept");
  }

  return MQ_OK;
}

enum MqErrorE
SysGetSockOpt (
  struct MqS * const context,
  MQ_SOCK const socket,
  int const level,
  int const optname,
  MQ_sockopt_optval_T optval,
  socklen_t * const optlen
)
{
  if (unlikely (getsockopt (socket, level, optname, optval, optlen) == SOCKET_ERROR)) {
    return sSysMqErrorMsg (context, __func__, "getsockopt");
  }

  return MQ_OK;
}

enum MqErrorE
SysSetSockOpt (
  struct MqS * const context,
  MQ_SOCK const socket,
  int const level,
  int const optname,
  const MQ_sockopt_optval_T const optval,
  socklen_t const optlen
) {
  if (unlikely (setsockopt (socket, level, optname, optval, optlen) == SOCKET_ERROR)) {
    return sSysMqErrorMsg (context, __func__, "setsockopt");
  }

  return MQ_OK;
}

/*
enum MqErrorE
SysSetSockDebug(context, socket)
    struct MqS *  const   context; 
    MQ_SOCK                 socket;
{
    int i=1;
    if (unlikely(setsockopt(socket, SOL_SOCKET, SO_DEBUG, (char*) &i, sizeof(int)) == SOCKET_ERROR)) {
	return sSysMqErrorMsg(context, __func__, "setsockopt");
    }

    return MQ_OK;
}  
*/

enum MqErrorE
SysConnect (
  struct MqS * const context,
  MQ_SOCK const socket,
  struct sockaddr * const serv_addr,
  socklen_t const addrlen,
  MQ_TIME_T const timeout
)
{
  MQ_TIME_T startT = time (NULL);
  do {
    sSysSetErrorNum(0);
    if (unlikely (connect (socket, serv_addr, addrlen) == SOCKET_ERROR)) {
      switch (sSysGetErrorNum) {
	// this is for -> performance
        case WIN32_WSA(EINPROGRESS):
	  break;
        case WIN32_WSA(EISCONN):
	  return MQ_OK;
	case 0:	// this seems to be special in SOLARIS
        case WIN32_WSA (EWOULDBLOCK):
          // case WIN32_WSA(EALREADY):
        case WIN32_WSA (EALREADY):
        case WIN32_WSA (ECONNABORTED):
        case WIN32_WSA (ECONNREFUSED):
        case WIN32_WSA (EINVAL):
        case WIN32_WSA (ENOENT):
	  // if the server is not available try as often as 'timeout' allows
          pUSleep (context, 200000L);
          break;
        default:
	  return sSysMqErrorMsg (context, __func__, "connect");
          break;
      }
    } else {
      // connection found -> continue with OK
      return MQ_OK;
    }
  }
  while ((time (NULL) - startT) <= timeout);
  return MqErrorDbV (MQ_ERROR_TIMEOUT, timeout);
}

enum MqErrorE
SysSocket (
     struct MqS * const context,     
     int domain,
     int type,
     int protocol,
     MQ_SOCK *sock              
) {
  int oldflags;
  if (unlikely ((*sock = (MQ_SOCK) socket (domain, type, protocol)) == INVALID_SOCKET)) {
    return sSysMqErrorMsg (context, __func__, "socket");
  }

  //  This FD_CLOEXEC is required in trans.test using fixed --port test:
  //  > Nhi1Exec trans.test --full-testing --only-c --only-tcp --port 5678 --only-spawn \
  //	-only-num 1 --block-1 -match "trans-[14]-*" --debug 9
  //  without FD_CLOEXEC on server socket the client of "trans-2" connects to the spawed child
  //  of the previous killed server "Filter4" and receive immideatly an socket shutdown request.
  if (MQ_IS_SERVER(context)) {
    if (unlikely ((oldflags = fcntl (*sock, F_GETFD, 0)) == -1)) {
      return sSysMqErrorMsg (context, __func__, "fcntl F_GETFD");
    }
    oldflags |= FD_CLOEXEC;
    if (unlikely (fcntl (*sock, F_SETFD, oldflags) == -1)) {
      return sSysMqErrorMsg (context, __func__, "fcntl F_SETFD");
    }
  }
  return MQ_OK;
}

enum MqErrorE
SysSetAsync (
  struct MqS * const context,
  MQ_SOCK socket
)
{
  unsigned long curMqState = 1;

  if (unlikely (WIN32_socket (ioctl) (socket, FIONBIO, &curMqState) == -1)) {
    return sSysMqErrorMsg (context, __func__, "ioctl");
  }

  return MQ_OK;
}

enum MqErrorE
SysBind (
     struct MqS * const context,     
     MQ_SOCK socket,
     const struct sockaddr *my_addr,
     const socklen_t addrlen
) {
  MQ_INT num = (MQ_INT) context->config.io.timeout;
  while (1) {
    if (unlikely (bind (socket, my_addr, addrlen) == SOCKET_ERROR)) {
      enum MqErrorE ret = MQ_OK;
      // handle: ERRNO<98> ERR<Address already in use>
      if (my_addr->sa_family == AF_INET) {
	MQ_CST host;
	MQ_INT port;
	if (sSysGetErrorNum == WIN32_WSA(EADDRINUSE)) {
	  MqLal.SysSleep(MQ_ERROR_IGNORE, 1);
	  if (--num > 0) {
	    MqDLogV(context,1,"%s - wait for %i seconds\n",strerror (sSysGetErrorNum), num);
	    sSysSetErrorNum(0);
	    continue;
	  }
	}
	ret = sSysMqErrorMsg (context, __func__, "bind");
	if (!MqErrorCheckI(SysGetTcpInfo(MQ_ERROR_IGNORE,(struct sockaddr_in*)my_addr,&host,&port))) {
	  MqErrorSAppendV(context,"%s socket host<%s> and port<%u>", (MQ_IS_SERVER(context)?"local":"remote"),host, port);
	}
      } else {
	ret = sSysMqErrorMsg (context, __func__, "bind");
      }
      return ret;
    }
    break;
  }

  return MQ_OK;
}

enum MqErrorE
SysSelect (
  struct MqS * const context,
  int n,
  fd_set * readfds,
  fd_set * writefds,
  struct timeval const * const timeout
)
{
  int ret;
  struct timeval tv = *timeout;
  sSysSetErrorNum(0);
  ret = MqSysSelect (n, (void*)readfds, (void*)writefds, NULL, &tv);
  switch (ret) {
    case 0:
      return MQ_CONTINUE;   // timeout
    case SOCKET_ERROR:
      switch (sSysGetErrorNum) {
        /*
#ifdef MQ_IS_WIN32
	case WIN32_WSA (ENOTSOCK):
          M0
          MqSysSleep(context,1);
          return MQ_CONTINUE;
        case WIN32_WSA (EFAULT):
          M1
          MqSysSleep(context,1);
          return MQ_CONTINUE;
#endif
          */
	case WIN32_WSA (EINTR):
          return MQ_CONTINUE;
	//case WIN32_WSA (ENOTSOCK):
        //case WIN32_WSA (EFAULT):
	case WIN32_WSA (EBADF): {
	  // check all sockets's to get the bad one
	  MQ_SOCK i;
	  int num=0;
	  fd_set fd_test, *fds = NULL, *read_test = NULL, *write_test = NULL;
	  struct timeval timeout = {0L,0L};
	  struct MqS const * sockmq = NULL;
	  MQ_SOCK sock=-1;
	  // read or write ?
	  if (readfds != NULL) {
	    read_test = &fd_test;
	    fds = readfds;
	  } else {
	    write_test = &fd_test;
	    fds = writefds;
	  }
	  // get the number of sockets
	  for (i=0;i<n;i++) {
	    if (FD_ISSET(i,fds)) {
	      num++;
	      sock = i;
              //printI(sock)
	    }
	  }
          // only one socket?
	  if (num == 1) {
            sockmq = pIoGetMsgqueFromSocket(context->link.io,sock);
	    if (sockmq == NULL) {
	      return sSysMqErrorMsg (context, __func__, "select");
	    } else if (sockmq == context) {
	      pIoCloseSocket (__func__, context->link.io);
	      return pErrorSetExitWithCheck (context);
	    } else {
	      pIoCloseSocket (__func__, sockmq->link.io);
	      return MQ_CONTINUE;
	    }
	  }
	  // ok, more than one socket check every socket
	  for (i=0;i<n;i++) {
	    if (!FD_ISSET(i,fds)) continue;
	    FD_ZERO (&fd_test);
	    FD_SET (i, &fd_test);
	    if (MqErrorCheckI (SysSelect(context,i+1,read_test,write_test,&timeout))) {
	      return MQ_ERROR;
	    }
	  }
	  return MQ_CONTINUE;
	}
	default:
          return sSysMqErrorMsg (context, __func__, "select");
      }
  }
  return MQ_OK;
}

enum MqErrorE
SysListen (
     struct MqS * const context,     
     MQ_SOCK socket,
     int backlog
) {
  if (unlikely (listen (socket, backlog) == SOCKET_ERROR)) {
    return sSysMqErrorMsg (context, __func__, "listen");
  }

  return MQ_OK;
}

enum MqErrorE
SysGetSockName (
  struct MqS * const context,     
  MQ_SOCK socket,            
  struct sockaddr *sockaddr, 
  socklen_t *sockaddrlen    
) {

  if (unlikely (getsockname (socket, sockaddr, sockaddrlen) == SOCKET_ERROR)) {
    return sSysMqErrorMsg (context, __func__, "getsockname");
  }

  return MQ_OK;
}

enum MqErrorE
SysCloseSocket (
  struct MqS * const context,
  MQ_CST const caller,
  MQ_BOL doShutdown,
  MQ_SOCK * hdlP
)
{
  enum MqErrorE ret = MQ_OK;
  if (*hdlP < 0) return MQ_OK;
  if (doShutdown) {
    MqDLogV(context,5,"%s - shutdown socket<%i>\n", caller, *hdlP);
    if (unlikely (shutdown (*hdlP, SHUTDOWN_BOTH) == SOCKET_ERROR)) {
      switch (sSysGetErrorNum) {
	case WIN32_WSA (ENOTCONN):
	case WIN32_WSA (EBADF):
	case WIN32_WSA (EINVAL):
	  break;
	default:
	  ret = sSysMqErrorMsg (context, __func__, "shutdown (socket)");
	  goto error;
      }
    }
  }

  MqDLogV(context,5,"%s - close socket<%i>\n", caller, *hdlP);
  if (unlikely (WIN32_socket (close) (*hdlP) == SOCKET_ERROR)) {
    ret = sSysMqErrorMsg (context, __func__, "close (socket)");
    goto error;
  }

error:
  *hdlP *= -1;
  return ret;
}

enum MqErrorE
SysSend (
  struct MqS * const context,
  MQ_SOCK hdl,
  MQ_BIN buf,
  MQ_SIZE numBytes,
  int flags,
  MQ_TIME_T timeout
)
{
  MQ_SIZE ldata = 0;
  MQ_INT trycount = (MQ_INT) (timeout*100);

  // send data from buf
  do {
    ldata = send (hdl, (const MQ_buf_T) buf, numBytes, flags);
//printLV("numBytes<%d>, hdl<%d>, ldata<%d>\n", numBytes, hdl, ldata);
    // check for errors
    if (unlikely (ldata == -1)) {
//printLV("ERROR sock<%i>, numBytes<%i>, errnum<%i>, str<%s>\n", hdl, numBytes, sSysGetErrorNum, strerror(errno));
      switch (sSysGetErrorNum) {
        case WIN32_WSA (EWOULDBLOCK): {
	  // waiting for "send" is buggy -> just use 0.01 sec and try send again on "timeout" (MQ_CONTINUE)
	  struct timeval tv = {(long) 0L, 10000L};
	  fd_set fds;
          if (trycount <= 0) return MqErrorDbV (MQ_ERROR_TIMEOUT, timeout);
	  // now wait until the socket become send-able
	  FD_ZERO(&fds);
	  FD_SET(hdl, &fds);
	  switch (SysSelect (context, (hdl+1), NULL, &fds, &tv)) {
	    case MQ_OK:
	      break;
	    case MQ_CONTINUE:
	      trycount -= 1;
	      continue; // with "do" loop
	    case MQ_ERROR:
	      pIoCloseSocket (__func__, context->link.io);
	      return MqErrorStack (context);
	  }
	  ldata = 0;
	  break;
	}
	case WIN32_WSA (ECONNRESET):
	case WIN32_WSA (EBADF): {
          pIoCloseSocket (__func__, context->link.io);
	  return pErrorSetExitWithCheck (context);
	}
        default:
          pIoCloseSocket (__func__, context->link.io);
          return sSysMqErrorMsg (context, __func__, "send");
      }
    }
    buf += ldata;
    numBytes -= ldata;
  }
  while (numBytes > 0);
  return MQ_OK;
}

enum MqErrorE
SysRecv (
  struct MqS * const context,
  MQ_SOCK const hdl,
  MQ_BIN buf,
  MQ_SIZE numBytes,
  MQ_SIZE * const newSize,
  MQ_TIME_T timeout
)
{
  int const flags = 0;
  register MQ_SIZE ldata = 0;
  *newSize = 0;

  // recv data in buf
  do {
    ldata = recv (hdl, (MQ_buf_T) buf, numBytes, flags);

    // check for errors
    if (unlikely (ldata <= 0)) {
	if (ldata == -1) {
//MqDLogV(context,0,"ERROR sock<%i>, numBytes<%i>, str<%s>\n", hdl, numBytes, strerror(errno));
	  switch (sSysGetErrorNum) {
	    case WIN32_WSA (EWOULDBLOCK): {
	      struct timeval tv = {(long)timeout, 0L};
	      fd_set fds;
	      FD_ZERO(&fds);
	      FD_SET(hdl, &fds);
	      // now wait until the socket become recv-able
	      switch (SysSelect (context, (hdl+1), &fds, NULL, &tv)) {
		case MQ_OK:
		  break;
		case MQ_CONTINUE:
		  return MqErrorDbV (MQ_ERROR_TIMEOUT, timeout);
		case MQ_ERROR:
		  pIoCloseSocket (__func__, context->link.io);
		  return MqErrorStack(context);
	      }
	      ldata = 0;
	      break;
	    }
	    case WIN32_WSA (ECONNRESET):
	    case WIN32_WSA (EBADF): {
	      pIoCloseSocket (__func__, context->link.io);
	      return pErrorSetExitWithCheck (context);
	    }
	    default:
	      pIoCloseSocket (__func__, context->link.io);
	      return sSysMqErrorMsg (context, __func__, "recv");
	  }
	} else if (ldata == 0) {
	  pIoCloseSocket (__func__, context->link.io);
	  return pErrorSetExitWithCheck (context);
	}
    }

    buf += ldata;
    numBytes -= ldata;
    *newSize += ldata;
  }
  while (numBytes > 0);

  return MQ_OK;
}

/*****************************************************************************/
/*                                                                           */
/*                                sys_tcp                                    */
/*                                                                           */
/*****************************************************************************/

void
MqSysFreeAddrInfo(
    struct addrinfo **res		///< [in,out] the linked list of addrinfo data freed and set pointer to NULL
) {
  if (!*res) return;
  freeaddrinfo(*res);
  *res = NULL;
}

enum MqErrorE
SysGetAddrInfo(
    struct MqS * const context,		///< [in] handle error
    MQ_CST node,			///< [in] name of the host or \c NULL
    MQ_CST service,			///< [in] name of the port or \c NULL
    const struct addrinfo *hints,	///< [in] helper to restrict the amount of return values
    struct addrinfo **res		///< [out] the linked list of the results
) {
  int ret;
  if ((ret = getaddrinfo(node,service,hints,res)) != 0) {
    return MqErrorV (context, __func__, ret, "%s -> ERRNO<%i> ERR<%s>", "getaddrinfo", ret, gai_strerror(ret));
  }
  return MQ_OK;
}

enum MqErrorE
SysGetTcpInfo(
  struct MqS * const context,	    ///< [in] handle error
  struct sockaddr_in * addr,	    ///< [in] address information
  MQ_CST *host,			    ///< [out] the hostname from addrinfo
  MQ_INT *port			    ///< [out] the port number from addrinfo
) {
  // 2. check
  if (addr->sin_family != AF_INET)
    return MqErrorDbV (MQ_ERROR_CAN_NOT, "AF_INET");
  // 3. get host name
  if (unlikely ((*host = inet_ntoa (addr->sin_addr)) == NULL)) {
    return MqErrorDbV (MQ_ERROR_CAN_NOT, "inet_ntoa");
  }
  // 4. get port number
  *port = ntohs (addr->sin_port);

  return MQ_OK;
}

/*
enum MqErrorE
SysInetAton (
     struct MqS * const context,	    ///< handle error
     const MQ_STR host,			    ///< the hostname in numbers-and-dots notation
     struct in_addr *in_addr		    ///< OUT: the adress entry struct
) {
#if defined(MQ_IS_POSIX)
  if (unlikely (inet_aton (host, in_addr) == 0))
#elif defined(MQ_IS_WIN32)
  if (unlikely ((in_addr->s_addr = inet_addr (host)) == INADDR_NONE))
#else
#   error not supported
#endif
  {
    return MqErrorV (context, __func__, -1, "unknown host-address '%s'", host);
  }

  return MQ_OK;
}
*/

enum MqErrorE
SysSocketPair (
  struct MqS * const context,	    ///< [in] handle error
  int socks[]				    ///< [out] the result from socketpair
) {
#if defined(HAVE_SOCKETPAIR)
  int oldflags;
  sSysSetErrorNum(0);
  if (unlikely (socketpair (AF_UNIX, SOCK_STREAM, 0, socks) == -1)) {
    return sSysMqErrorMsg (context, __func__, "socketpair");
  }

  if (unlikely ((oldflags = fcntl (socks[0], F_GETFD, 0)) == -1)) {
    return sSysMqErrorMsg (context, __func__, "fcntl F_GETFD");
  }
  oldflags |= FD_CLOEXEC;
  if (unlikely (fcntl (socks[0], F_SETFD, oldflags) == -1)) {
    return sSysMqErrorMsg (context, __func__, "fcntl F_SETFD");
  }

  return MQ_OK;
#else
/* socketpair.c
 * Copyright 2007 by Nathan C. Myers <ncm@cantrip.org>; all rights reserved.
 * This code is Free Software.  It may be copied freely, in original or 
 * modified form, subject only to the restrictions that (1) the author is
 * relieved from all responsibilities for any use for any purpose, and (2)
 * this copyright notice must be retained, unchanged, in its entirety.  If
 * for any reason the author might be held responsible for any consequences
 * of copying or use, license is withheld.  
 */

/* dumb_socketpair:
 *   If make_overlapped is nonzero, both sockets created will be usable for
 *   "overlapped" operations via WSASend etc.  If make_overlapped is zero,
 *   socks[0] (only) will be usable with regular ReadFile etc., and thus 
 *   suitable for use as stdin or stdout of a child process.  Note that the
 *   sockets must be closed with closesocket() regardless.
 */
    struct sockaddr_in addr;
    MQ_SOCK listener;
    socklen_t addrlen = sizeof(addr);

    socks[0] = socks[1] = (int) INVALID_SOCKET;
    MqErrorCheck (SysSocket(context,AF_INET,SOCK_STREAM,0,&listener));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_port = 0;

    MqErrorCheck (SysBind(context,listener, (const struct sockaddr*) &addr, sizeof(addr)));
    MqErrorCheck (SysGetSockName(context, listener, (struct sockaddr*) &addr, &addrlen));

    do {
	MqErrorCheck (SysListen(context,listener,1));
	MqErrorCheck (SysSocket(context,AF_INET,SOCK_STREAM,0,&socks[0]));
	MqErrorCheck (SysConnect(context,socks[0],(struct sockaddr*const) &addr, sizeof(addr),1));
	MqErrorCheck (SysAccept(context,listener,NULL,NULL,&socks[1]));
	MqErrorCheck (SysCloseSocket(context,__func__,MQ_NO,&listener));
        return MQ_OK;
    } while (0);

error:
  SysCloseSocket(MQ_ERROR_IGNORE,__func__,MQ_NO,&listener);
  return MqErrorStack (context);
#endif
}

END_C_DECLS

