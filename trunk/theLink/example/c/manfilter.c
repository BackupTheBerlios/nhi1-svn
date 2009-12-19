/**
 *  \file       theLink/example/c/manfilter.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include "string.h"
#include "msgque.h"
static enum MqErrorE  FTR( struct MqS *ctx, MQ_PTR data) {
  MQ_CST str;
  struct MqS * ftr;
  MqErrorCheck (MqConfigGetFilter (ctx, &ftr));
  MqSendSTART(ftr);
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqBufferSetV(ctx->temp,"<%s>", str);
    MqSendU(ftr, ctx->temp);
  }
  MqErrorCheck (MqSendEND_AND_WAIT(ftr, "+FTR", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (ctx);
}
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqConfigSetName (ctx, "ManFilter");
  MqConfigSetDefaultFactory (ctx);
  MqConfigSetIsServer (ctx, MQ_YES);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqServiceCreate (ctx, "+FTR", FTR, NULL, NULL));
  MqErrorCheck (MqServiceProxy  (ctx, "+EOF"));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
