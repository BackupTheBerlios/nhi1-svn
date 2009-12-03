/**
 *  \file       theLink/example/c/mulclient.c
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
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MQ_DBL d;
  MqConfigSetName (ctx, "MyMul");
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqSendSTART(ctx);
  MqSendD(ctx,3.67);
  MqSendD(ctx,22.3);
  MqSendEND_AND_WAIT(ctx, "MMUL", 10);
  MqErrorCheck (MqReadD(ctx, &d));
  printf("%f\n", d);
error:
  MqExit(ctx);
}
