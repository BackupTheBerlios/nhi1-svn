/**
 *  \file       theLink/example/c/testclient.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "mqconfig.h"
#include "msgque.h"
#include "debug.h"
#include <stdlib.h>
#include <string.h>

#define MQ_CONTEXT_S ctx

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
#ifdef MQ_IS_POSIX
  struct MqBufferLS *bs0 = MqBufferLCreateArgsV(NULL, "c0", "@", "../example/c/testserver", NULL);
#else
  struct MqBufferLS *bs0 = MqBufferLCreateArgsV(NULL, "c0", "@", "../example/c/testserver.exe", NULL);
#endif
  const char *debug = getenv("TS_DEBUG");
  struct MqBufferLS *args;

  ctx = MqContextCreate(0, NULL);
  MqConfigSetName(ctx, "c0");
  MqConfigSetSrvName(ctx, "s0");
  MqConfigSetDebug(ctx, debug != NULL ? atoi(debug) : 0);
  MqErrorCheck (MqLinkCreate(ctx, &bs0));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx,&bs0));
  c0 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c00", "--srvname", "s00", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c0, &args));
  c00 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c01", "--srvname", "s01", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c0, &args));
  c01 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c000", "--srvname", "s000", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c00, &args));
  c000 = ctx;

  ctx = MqContextCreate(0, NULL);
  MqConfigSetName(ctx, "c1");
  MqConfigSetSrvName(ctx, "s1");
  MqConfigSetDebug(ctx, debug != NULL ? atoi(debug) : 0);
  MqErrorCheck (MqLinkCreate (ctx, &largv));
  MqErrorCheck (MqCheckForLeftOverArguments(ctx, &largv));
  c1 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c10", "--srvname", "s10", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c1, &args));
  c10 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c100", "--srvname", "s100", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c10, &args));
  c100 = ctx;

  ctx = MqContextCreate(0, NULL);
  args = MqBufferLCreateArgsV (ctx, "xx", "--name", "c101", "--srvname", "s101", NULL);
  MqErrorCheck (MqLinkCreateChild (ctx, c10, &args));
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
