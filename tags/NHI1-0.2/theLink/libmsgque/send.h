/**
 *  \file       theLink/libmsgque/send.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef SEND_H
#   define SEND_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                           send_private_init                               */
/*                                                                           */
/*****************************************************************************/

/** -brief creation of an  \e MqSendS object
 */
enum MqErrorE pSendCreate (
  struct MqS * const context,
  struct MqSendS ** const out
) __attribute__((nonnull));

/** -brief deletion of an \e MqSendS object
 */
void pSendDelete (
  struct MqSendS ** sendP
) __attribute__((nonnull));

enum MqErrorE pSendSYSTEM (
  struct MqS * const context,
  MQ_CST const token
);

enum MqErrorE pMqSendEND (
  struct MqS * const context,
  MQ_CST const token,
  const MQ_HDL trans
);

enum MqErrorE pSendSYSTEM_RETR (
  struct MqS * const context
);

END_C_DECLS

#endif /* SEND_H */




