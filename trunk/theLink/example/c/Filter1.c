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
  return MqSendRETURN (ctx);
}

static enum MqErrorE EOF_F( struct MqS *ctx, MQ_PTR data) {
  MQ_CST dat;
  MQ_BUF buf = NULL;
  struct MqS * ftr;
  MqErrorCheck (MqServiceGetFilter (ctx, 0, &ftr));
  MqSendSTART(ftr);
  while (myftr->cursize) {
    MqErrorCheck(MqBufferLGetU(ctx, myftr, 0, &buf));
    MqErrorCheck(MqBufferGetC(buf, &dat));
    if (!strncmp(dat, "END", 3)) {
      MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+FTR", MQ_TIMEOUT_USER));
      MqSendSTART(ftr);
    } else {
      MqSendC(ftr, dat);
    }
  }
  MqBufferDelete (&buf);
  MqErrorCheck (MqSendEND_AND_WAIT (ftr, "+EOF", MQ_TIMEOUT_USER));
error:
  return MqSendRETURN (ctx);
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
  MqErrorCheck (MqServiceCreate (ctx, "+EOF", EOF_F, NULL, NULL));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqErrorCheck (MqProcessEvent(ctx,MQ_TIMEOUT,MQ_WAIT_FOREVER));
error:
  MqExit(ctx);
}
