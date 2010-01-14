/**
 *  \file       theLink/example/csharp/mulserver.cs
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
  sealed class mulserver : MqS, IServerSetup, IFactory {
    MqS IFactory.Factory() {
      return new mulserver();
    }
    public void MMUL () {
      SendSTART();
      SendD(ReadD() * ReadD());
      SendRETURN();
    }
    void IServerSetup.ServerSetup() {
      ServiceCreate("MMUL", MMUL);
    }
    static void Main(string[] argv) {
      mulserver srv = new mulserver();
      try {
	srv.ConfigSetName("MyMulServer");
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}
