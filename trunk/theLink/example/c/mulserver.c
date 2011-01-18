/**
 *  \file       theLink/example/c/mulserver.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque.h"
#include "string.h"

static enum MqErrorE  MMUL( struct MqS *ctx, MQ_PTR data) {
  MQ_DBL d1,d2;
  MqErrorCheck (MqSendSTART(ctx));
  MqErrorCheck (MqReadD(ctx, &d1));
  MqErrorCheck (MqReadD(ctx, &d2));
  MqErrorCheck (MqSendD(ctx, d1*d2));
error:
  return MqSendRETURN(ctx);
}
static enum MqErrorE ServerSetup (struct MqS *ctx, MQ_PTR data) {
  return MqServiceCreate(ctx,"MMUL", MMUL, NULL, NULL);
}
static enum MqErrorE
MulServerFactory (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS * const item,
  struct MqS **contextP
)
{ 
  struct MqS * const ctx = *contextP = MqContextCreate(0,tmpl);
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  return MQ_OK;
}
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqFactoryErrorPanic(MqFactoryNew("mulserver", MulServerFactory, NULL, NULL, NULL, NULL, NULL, NULL, &ctx));
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
