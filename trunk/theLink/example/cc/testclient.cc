/**
 *  \file       theLink/example/cc/testclient.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include  <iostream>
#include  <string>
#include  <cstring>
#include  <cstdlib>
#include  "ccmsgque.h"
#include  "debug.h"

using namespace std;
using namespace ccmsgque;

static string Get( MqC& ctx) {
  MqBufferC RET = ctx.ContextGetBuffer();
  ctx.SendSTART();
  ctx.SendEND_AND_WAIT("GTCX", MQ_TIMEOUT_USER);
  RET.Reset();
  RET.AppendC(ctx.ConfigGetName());
  RET.AppendC("+");
  RET.AppendC(ctx.ReadC());
  RET.AppendC(ctx.ReadC());
  RET.AppendC(ctx.ReadC());
  RET.AppendC(ctx.ReadC());
  RET.AppendC(ctx.ReadC());
  RET.AppendC(ctx.ReadC());
  return RET.GetC();
}

int MQ_CDECL main(int argc, MQ_CST argv[]) {

    // setup the clients
  MQ_CST t1[] =	  {argv[0], "--name", "c1", "--srvname", "s1"};
  vector<MQ_CST> LIST(t1, t1+5);

  if (argc > 1)
    LIST.insert(LIST.end(), argv+1, argv+argc);

  static MqC c0, c00, c01, c000, c1, c10, c100, c101;

  try {

    // create the "LINK"
#ifdef _WIN32
# define SRV  "../example/cc/testserver.exe"
#else
# define SRV  "../example/cc/testserver"
#endif

    c0.LinkCreateV(argv[0], "--name", "c0", "--debug", getenv("TS_DEBUG"), "@", SRV, "--name", "s0", NULL);
    c00.LinkCreateChildV(c0, argv[0], "--name", "c00", "--srvname", "s00", NULL);
    c01.LinkCreateChildV(c0, argv[0], "--name", "c01", "--srvname", "s01", NULL);
    c000.LinkCreateChildV(c00, argv[0], "--name", "c000", "--srvname", "s000", NULL);
    c1.LinkCreateVT(LIST);
    c10.LinkCreateChildV(c1, argv[0], "--name", "c10", "--srvname", "s10", NULL);
    c100.LinkCreateChildV(c10, argv[0], "--name", "c100", "--srvname", "s100", NULL);
    c101.LinkCreateChildV(c10, argv[0], "--name", "c101", "--srvname", "s101", NULL);

    // do the tests
    cout  <<  Get(c0)	<< endl
	  <<  Get(c00)	<< endl
	  <<  Get(c01)	<< endl
	  <<  Get(c000)	<< endl;

    cout  <<  Get(c1)	<< endl
	  <<  Get(c10)	<< endl
	  <<  Get(c100)	<< endl
	  <<  Get(c101)	<< endl;

  } catch (const exception& e) {
    c1.ErrorSet(e);
  }

  // do the cleanup
  c1.Exit();
}
