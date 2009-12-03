/**
 *  \file       example/csharp/mulserver.cs
 *  \brief      \$Id: mulserver.cs 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;
namespace example {
  sealed class mulserver : MqS, IServerSetup, IFactory {
    MqS IFactory.Call() {
      return new mulserver();
    }
    public void MMUL () {
      SendSTART();
      SendD(ReadD() * ReadD());
      SendRETURN();
    }
    void IServerSetup.Call() {
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
