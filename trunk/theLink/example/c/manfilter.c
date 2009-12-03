/**
 *  \file       theLink/example/c/manfilter.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include "string.h"
#include "msgque.h"
static enum MqErrorE  FTR( struct MqS *ctx, MQ_PTR data) {
  MQ_CST str;
  MqSendSTART(ctx);
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqBufferSetV(ctx->temp,"<%s>", str);
    MqSendU(ctx, ctx->temp);
  }
  return MqSendFTR(ctx, 10);
error:
  return MqErrorStack (ctx);
}
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqConfigSetName (ctx, "ManFilter");
  MqConfigSetFilterFTR (ctx, FTR, NULL, NULL, NULL);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
