/**
 *  \file       libmsgque/error.h
 *  \brief      \$Id: error.h 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef ERROR_H
#   define ERROR_H

BEGIN_C_DECLS

void pErrorSetup (
  struct MqS * const context
);

void pErrorCleanup (
  struct MqS * const context
);

enum MqErrorE pErrorSet (
  struct MqS * const context,
  MQ_INT const errnum,
  MQ_CST const errtext,
  MQ_BOL const append
);

void pErrorAppendC (
  struct MqS * const context,
  MQ_CST const msg
);

void
pErrorReport(
  struct MqS * const context
);

enum MqErrorE
pErrorSetEXIT (
  struct MqS * const context,
  MQ_CST prefix
);

void
pErrorSync (
  struct MqS * const out,
  struct MqS * const in
);

#define MqErrorDb(item) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,(item+200),MqMessageText[item])

#define MqErrorDb2(context,item) \
    MqErrorSGenV(context,__func__,MQ_ERROR,(item+200),MqMessageText[item])

#define MqErrorDbV(item, ...) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,(item+200),MqMessageText[item], __VA_ARGS__)

#define MqErrorDbV2(context,item, ...) \
    MqErrorSGenV(context,__func__,MQ_ERROR,(item+200),MqMessageText[item], __VA_ARGS__)

END_C_DECLS

#endif /* ERROR_H */


