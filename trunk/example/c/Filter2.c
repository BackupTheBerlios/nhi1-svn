/**
 *  \file       example/c/Filter2.c
 *  \brief      \$Id: Filter2.c 507 2009-11-28 15:18:46Z dev1usr $
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
#include "debug.h"

static struct MqBufferLS * myftr;

static enum MqErrorE FTR_F( struct MqS *ctx, MQ_PTR data) {
  return MqErrorC (ctx, "ErrorSet", -1, "my error");
}

int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  myftr = MqBufferLCreate(10);
  MqConfigSetName (ctx, "filter");
  MqConfigSetFilterFTR (ctx, FTR_F, NULL, NULL, NULL);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
