/**
 *  \file       theLink/libmsgque/error.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
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

void
pErrorSync (
  struct MqS * const out,
  struct MqS * const in
);

enum MqErrorE MQ_DECL pErrorSetExitWithCheckP (
  struct MqS * const context,
  MQ_CST const prefix
);
#define pErrorSetExitWithCheck(ctx) pErrorSetExitWithCheckP(ctx,__func__)


void pErrorReset (struct MqS * const);

#define MqErrorDb(item) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item])

#define MqErrorDb2(context,item) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item])

#define MqErrorDbV(item, ...) \
    MqErrorSGenV(MQ_CONTEXT_S,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item], __VA_ARGS__)

#define MqErrorDbV2(context,item, ...) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(item),MqMessageText[item], __VA_ARGS__)

#define MqErrorDbFactoryMsg(context,msg) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(MQ_ERROR_FACTORY),MqMessageText[MQ_ERROR_FACTORY],msg)

#define MqErrorDbFactoryNum(context,num) \
    MqErrorSGenV(context,__func__,MQ_ERROR,MqMessageNum(MQ_ERROR_FACTORY),MqMessageText[MQ_ERROR_FACTORY],MqFactoryReturnMsg(num))

END_C_DECLS

#endif /* ERROR_H */



