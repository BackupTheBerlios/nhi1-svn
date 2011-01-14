/**
 *  \file       theLink/example/cc/mulserver.cc
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

class mulserver : public MqC, public IServerSetup {
  public:
    mulserver(MqS *tmpl) : MqC(tmpl) {}
  private:
    void ServerSetup () {
      ServiceCreate("MMUL", CallbackF(&mulserver::MMUL));
    }
    void MMUL () {
      SendSTART();
      SendD(ReadD() * ReadD());
      SendRETURN();
    }
};

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  mulserver *ctx = MqFactoryC<mulserver>::New("MyMulServer");
  try {
    ctx->LinkCreateVC (argc, argv);
    ctx->ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    ctx->ErrorSet(e);
  }
  ctx->Exit ();
}
