/**
 *  \file       example/c/mulserver.c
 *  \brief      \$Id: mulserver.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include "string.h"
#include "msgque.h"
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
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MqConfigSetName (ctx, "MyMulServer");
  MqConfigSetServerSetup (ctx, ServerSetup, NULL, NULL, NULL);
  MqConfigSetDefaultFactory (ctx);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
