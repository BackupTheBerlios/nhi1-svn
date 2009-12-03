/**
 *  \file       theLink/libmsgque/generic_io.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef GENERIC_IO_H
#define GENERIC_IO_H

#define GENERIC_SAVE_ERROR(generiC) ((generiC)&&(generiC)->error?(generiC)->error:MQ_ERROR_IGNORE)

// this is needed for "sockaddr" and "socklen_t"
#include "sys_com.h"

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                                data                                       */
/*                                                                           */
/*****************************************************************************/

/// \brief everything \e Generic needs for local storage
struct GenericS {
  struct MqIoS * io;		///< link to 'io' object
  struct MqS * context;  ///< my MqErrorS
  MQ_SOCK sock;                 ///< the socket
  char stringbuffer[20];	///< context temporar memory
};

/*****************************************************************************/
/*                                                                           */
/*                                set/get                                    */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/*                             generic_io				     */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE GenericCreate (
  struct MqIoS * const io,
  struct GenericS ** const out
);

void GenericDelete (
  struct GenericS ** const genericP
) __attribute__((nonnull));

enum MqErrorE GenericCreateSocket (
  struct GenericS * const generiC,
  int const domain,
  int const type,
  int protocol
);

enum MqErrorE GenericServer (
  struct GenericS * const generiC,	///< the current generic handle
  struct sockaddr * const sockaddr,	///< the address package
  socklen_t const sockaddrle		///< the length of the address
);

enum MqErrorE GenericConnect (
  struct GenericS * const generiC,
  struct sockaddr * const sockaddr,
  socklen_t const sockaddrlen,
  MQ_TIME_T timeout
);

enum MqErrorE GenericBind (
  struct GenericS const * const generiC,    ///< the current generic handle
  struct sockaddr *sockaddr,		    ///< the address package
  socklen_t sockaddrlen 		    ///< the length of the address
);

enum MqErrorE GenericGetSockName (
  struct GenericS const * const generiC,    ///< the current generic handle
  struct sockaddr *sockaddr,		    ///< the address package
  socklen_t *sockaddrlen 		    ///< the length of the address
);

END_C_DECLS

#endif /* GENERIC_IO_H */

