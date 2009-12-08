/**
 *  \file       theLink/example/c/Filter1.c
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

static enum MqErrorE FTR_F( struct MqS *ctx, MQ_PTR data) {
  MQ_CST str;
  while (MqReadItemExists(ctx)) {
    MqErrorCheck (MqReadC(ctx, &str));
    MqBufferLAppendU(myftr,  MqBufferSetV(MqBufferCreate(ctx,10),"<%s>", str));
  }
  MqBufferLAppendC(myftr, "END");
error:
  return MqErrorStack (ctx);
}

static enum MqErrorE EOF_F( struct MqS *ctx, MQ_PTR data) {
  MQ_CST dat;
  MQ_BUF buf = NULL;
  MqSendSTART(ctx);
  while (myftr->cursize) {
    MqErrorCheck(MqBufferLGetU(ctx, myftr, 0, &buf));
    MqErrorCheck(MqBufferGetC(buf, &dat));
    if (!strncmp(dat, "END", 3)) {
      MqSendFTR(ctx, 10);
      MqSendSTART(ctx);
    } else {
      MqSendC(ctx, dat);
    }
  }
  MqBufferDelete (&buf);
  return MqSendFTR(ctx, 10);
error:
  return MqErrorStack (ctx);
}

int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  myftr = MqBufferLCreate(10);
  MqConfigSetName (ctx, "filter");
  MqConfigSetFilterFTR (ctx, FTR_F, NULL, NULL, NULL);
  MqConfigSetFilterEOF (ctx, EOF_F, NULL, NULL, NULL);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
