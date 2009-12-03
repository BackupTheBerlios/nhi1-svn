/**
 *  \file       theLink/example/csharp/testserver.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {

  sealed class testserver : MqS, IServerSetup, IFactory {

    MqS IFactory.Call() {
      return new testserver();
    }

    public void GTCX () {
      SendSTART();
      SendI(ConfigGetCtxId());
      SendC("+");
      if (ConfigGetIsParent()) {
        SendI(-1);
      } else {
        SendI(ConfigGetParent().ConfigGetCtxId());
      }
      SendC("+");
      SendC(ConfigGetName());
      SendC(":");
      SendRETURN();
    }

    void IServerSetup.Call() {
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
