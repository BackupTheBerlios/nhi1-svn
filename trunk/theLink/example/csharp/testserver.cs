/**
 *  \file       theLink/example/csharp/testserver.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {

  sealed class testserver : MqS, IServerSetup {

    public testserver(MqS tmpl) : base(tmpl) {
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
      testserver srv = MqFactoryS<testserver>.Add().New();
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

