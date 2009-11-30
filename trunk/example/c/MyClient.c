/**
 *  \file       example/c/MyClient.c
 *  \brief      \$Id: MyClient.c 507 2009-11-28 15:18:46Z dev1usr $
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
int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS * ctx = MqContextCreate(0, NULL);
  MQ_CST c;
  MqConfigSetName(ctx, "MyClient");
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  MqSendSTART(ctx);
  MqSendEND_AND_WAIT(ctx, "HLWO", 10);
  MqErrorCheck (MqReadC(ctx, &c));
  printf("%s\n", c);
error:
  MqExit(ctx);
}
