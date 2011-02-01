/**
 *  \file       theLink/example/cc/testserver.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ccmsgque.h"

using namespace ccmsgque;

class testserver : public MqC, public IServerSetup {
  public:
    testserver(MqS *tmpl) : MqC(tmpl) {}
  private:
    void GTCX () {
      SendSTART();
      SendI(LinkGetCtxId());
      SendC("+");
      if (LinkIsParent()) {
	SendI(-1);
      } else {
	SendI(LinkGetParent()->LinkGetCtxId());
      }
      SendC("+");
      SendC(ConfigGetName());
      SendC(":");
      SendRETURN();
    }
    void ServerSetup() {
      ServiceCreate("GTCX", CallbackF(&testserver::GTCX));
    }
};

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  testserver *ctx = MqFactoryC<testserver>::Add("testserver").New();
  try {
    ctx->LinkCreateVC(argc, argv);
    ctx->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    ctx->ErrorSet(e);
  }
  ctx->Exit ();
}

