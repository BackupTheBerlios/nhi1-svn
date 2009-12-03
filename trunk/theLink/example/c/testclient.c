/**
 *  \file       example/c/testclient.c
 *  \brief      \$Id: testclient.c 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include <stdlib.h>
#include <string.h>
#include "msgque.h"
#include "debug.h"

static const char * Get (
  struct MqS * const ctx
)
{
  static char str[250];
  MQ_CST buf;

  MqErrorCheck(MqSendSTART(ctx));
  MqErrorCheck(MqSendEND_AND_WAIT(ctx, "GTCX", MQ_TIMEOUT_USER));
  str[0] = '\0';
  strncat(str, MqConfigGetName(ctx), 250);
  strncat(str, "+", 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  MqErrorCheck(MqReadC(ctx, &buf)); strncat(str, buf, 250);
  return str;

error:
  MqExit(ctx);
  return "";
}

int main (int argc, MQ_CST argv[]) 
{
  struct MqBufferLS * largv = MqBufferLCreateArgs(argc, argv);
  struct MqS *ctx=NULL, *c0=NULL, *c00=NULL, *c01=NULL, *c000=NULL, *c1=NULL, *c10=NULL, *c100=NULL, *c101=NULL;
  struct MqBufferLS *bs0 = MqBufferLCreateArgsV(NULL, "c0", "@", "../example/c/testserver", NULL);
  const char *debug = getenv("TS_DEBUG");

  ctx = MqContextCreate(0, NULL);
  MqConfigSetName(ctx, "c0");
  MqConfigSetSrvName(ctx, "s0");
  MqConfigSetParent(ctx, c10);
  MqConfigSetDebug(ctx, debug != NULL ? atoi(debug) : 0);
  MqErrorCheck (MqLinkCreate (ctx, &bs0));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx,&bs0));
  c0 = ctx;

  ctx = MqContextCreate(0, c0);
  MqConfigSetName(ctx, "c00");
  MqConfigSetSrvName(ctx, "s00");
  MqConfigSetParent(ctx, c0);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c00 = ctx;

  ctx = MqContextCreate(0, c0);
  MqConfigSetName(ctx, "c01");
  MqConfigSetSrvName(ctx, "s01");
  MqConfigSetParent(ctx, c0);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c01 = ctx;

  ctx = MqContextCreate(0, c00);
  MqConfigSetName(ctx, "c000");
  MqConfigSetSrvName(ctx, "s000");
  MqConfigSetParent(ctx, c00);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c000 = ctx;

  ctx = MqContextCreate(0, NULL);
  MqConfigSetName(ctx, "c1");
  MqConfigSetSrvName(ctx, "s1");
  MqConfigSetDebug(ctx, debug != NULL ? atoi(debug) : 0);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  c1 = ctx;

  ctx = MqContextCreate(0, c1);
  MqConfigSetName(ctx, "c10");
  MqConfigSetSrvName(ctx, "s10");
  MqConfigSetParent(ctx, c1);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c10 = ctx;

  ctx = MqContextCreate(0, c10);
  MqConfigSetName(ctx, "c100");
  MqConfigSetSrvName(ctx, "s100");
  MqConfigSetParent(ctx, c10);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c100 = ctx;

  ctx = MqContextCreate(0, c10);
  MqConfigSetName(ctx, "c101");
  MqConfigSetSrvName(ctx, "s101");
  MqConfigSetParent(ctx, c10);
  MqErrorCheck (MqLinkCreate (ctx, NULL));
  c101 = ctx;

  // do the tests
  printf("%s\n", Get(c0));
  printf("%s\n", Get(c00));
  printf("%s\n", Get(c01));
  printf("%s\n", Get(c000));
  printf("%s\n", Get(c1));
  printf("%s\n", Get(c10));
  printf("%s\n", Get(c100));
  printf("%s\n", Get(c101));

  // do the cleanup
  MqContextDelete(&c0);
  MqContextDelete(&c00);
  MqContextDelete(&c01);
  MqContextDelete(&c000);
  MqContextDelete(&c1);
  MqContextDelete(&c10);
  MqContextDelete(&c100);
  MqContextDelete(&c101);
  return 0;

error:
  MqExit(ctx);
}
