/**
 */

#ifndef FACTORY_H
#   define FACTORY_H

BEGIN_C_DECLS

enum MqErrorE pSqlInsertSendTrans (
  struct MqS * const context, 
  MQ_TOK const callback, 
  MQ_BUF buf, 
  MQ_WID *transId
);

enum MqErrorE pSqlSelectSendTrans (
  struct MqS * const context,
  MQ_WID transId,
  MQ_BUF buf
);

enum MqErrorE pSqlInsertReadTrans (
  struct MqS * const context, 
  MQ_CST const ident, 
  MQ_WID const rmtTransId, 
  MQ_WID const oldTransId, 
  MQ_WID *transId
);

enum MqErrorE pSqlSelectReadTrans (
  struct MqS * const context, 
  MQ_WID transId
);

enum MqErrorE pSqlDeleteReadTrans (
  struct MqS * const context, 
  MQ_WID transId,
  MQ_WID *oldTransId
);

enum MqErrorE pSqlCreate (
  struct MqS * const context,
  struct MqSqlS ** const sql
);

void pSqlDelete (
  struct MqSqlS ** thread
);

END_C_DECLS

#endif /* FACTORY_H */

