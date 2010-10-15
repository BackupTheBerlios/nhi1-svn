/**
 *  \file       theLink/example/c/Filter2.c
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
#include "debug.h"

static struct MqBufferLS * myftr;

static enum MqErrorE FTR_F( struct MqS *ctx, MQ_CST prefix, MQ_PTR data) {
  return MqErrorC (ctx, "ErrorSet", -1, "my error");
}

int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  myftr = MqBufferLCreate(10);
  MqConfigSetName (ctx, "filter");
  MqConfigSetIsServer (ctx, MQ_YES);
  MqConfigSetDefaultFactory (ctx);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqServiceCreate (ctx, "+FTR", FTR_F, NULL, NULL));
  MqErrorCheck (MqServiceProxy  (ctx, "+EOF", 0));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
