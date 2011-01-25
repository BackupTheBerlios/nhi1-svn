/**
 *  \file       theLink/libmsgque/factory.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef FACTORY_H
#   define FACTORY_H

BEGIN_C_DECLS

/*****************************************************************************/
/*                                                                           */
/*                              token_misc                                   */
/*                                                                           */
/*****************************************************************************/

void pFactoryMark (
  struct MqS * const context,
  MqMarkF markF
);

void pFactoryCtxItemSet (
  struct MqS * const context,
  struct MqFactoryS * const item
);

enum MqErrorE pFactoryCtxInsertSendTrans (
  struct MqS * const context, 
  MQ_TOK const callback, 
  MQ_BUF buf, 
  MQ_WID *transId
);

enum MqErrorE pFactoryCtxSelectSendTrans (
  struct MqS * const context,
  MQ_WID transId,
  MQ_BUF buf
);

enum MqErrorE
pFactoryCtxInsertReadTrans (
  struct MqS * const context, 
  MQ_CST const ident, 
  MQ_WID const rmtTransId, 
  MQ_WID const oldTransId, 
  MQ_WID *transId
);

enum MqErrorE
pFactoryCtxSelectReadTrans (
  struct MqS * const context, 
  MQ_WID transId
);

enum MqErrorE
pFactoryCtxDeleteReadTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_WID *oldTransId
);

END_C_DECLS

#endif /* FACTORY_H */

