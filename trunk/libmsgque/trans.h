/**
 *  \file       libmsgque/trans.h
 *  \brief      \$Id: trans.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef TRANS_H
#   define TRANS_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              trans_init                                   */
/*                                                                           */
/*****************************************************************************/

/// \brief the status of a transaction used in the \e status member of the \e MqTransItemS object.
enum MqTransE {
  MQ_TRANS_START,               ///< start of transaction
  MQ_TRANS_END,                 ///< end of transaction
  MQ_TRANS_CANCEL               ///< transaction was canceled
};

enum MqErrorE pTransCreate (struct MqS * const, struct MqTransS ** const );
void pTransDelete (struct MqTransS ** const);

MQ_HDL pTransPop (
  struct MqTransS const * const trans,
  struct MqCallbackS callback
);

void pTransPush (
  struct MqTransS const * const trans,
  MQ_HDL transId
) __attribute__((nonnull));

/*****************************************************************************/
/*                                                                           */
/*                              trans_misc                                   */
/*                                                                           */
/*****************************************************************************/

int pTransCheckStart (
  struct MqTransS const * const trans,
  const MQ_HDL transId
);

/*****************************************************************************/
/*                                                                           */
/*                              trans_get                                    */
/*                                                                           */
/*****************************************************************************/

MQ_HDL pTransGetLast (
  struct MqTransS const * const trans,
  const MQ_HDL transId
);

struct MqReadS * pTransGetResult (
  struct MqTransS const * const trans,
  const MQ_HDL transId
);

enum MqTransE pTransGetStatus (
  struct MqTransS const * const trans,
  const MQ_HDL transId
);

/*****************************************************************************/
/*                                                                           */
/*                              trans_set                                    */
/*                                                                           */
/*****************************************************************************/

enum MqErrorE pTransSetResult (
  struct MqTransS const * const trans,
  enum MqTransE const status,
  struct MqReadS * const result
);

END_C_DECLS

#endif /* TRANS_H */



