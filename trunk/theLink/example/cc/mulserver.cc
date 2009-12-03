/**
 *  \file       example/cc/mulserver.cc
 *  \brief      \$Id: mulserver.cc 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ccmsgque.h"

using namespace ccmsgque;

class mulserver : public MqC, public IServerSetup, public IFactory {
  private:
    void MMUL () {
      SendSTART();
      SendD(ReadD() * ReadD());
      SendRETURN();
    }
    MqC* Factory() const {return new mulserver();}
    void ServerSetup () {
      ServiceCreate("MMUL", CallbackF(&mulserver::MMUL));
    }
};

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  mulserver ctx;
  try {
    ctx.ConfigSetName ("MyMulServer");
    ctx.LinkCreateVC (argc, argv);
    ctx.ProcessEvent ();
  } catch (const exception& e) {
    ctx.ErrorSet(e);
  }
  ctx.Exit ();
}
