/**
 *  \file       theLink/example/c/MyClient.c
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
