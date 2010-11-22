/**
 *  \file       theLink/example/go/src/testserver.go
 *  \brief      \$Id: testserver.go 320 2010-11-21 17:05:19Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 320 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {

  sealed class testserver : MqS, IServerSetup, IFactory {

    MqS IFactory.Factory() {
      return new testserver();
    }

    public void GTCX () {
      SendSTART();
      SendI(LinkGetCtxId());
      SendC("+");
      if (LinkIsParent()) {
        SendI(-1);
      } else {
        SendI(LinkGetParent().LinkGetCtxId());
      }
      SendC("+");
      SendC(ConfigGetName());
      SendC(":");
      SendRETURN();
    }

    void IServerSetup.ServerSetup() {
      ServiceCreate("GTCX", GTCX);
    }

    static void Main(string[] argv) {
      testserver srv = new testserver();
      try {
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}



