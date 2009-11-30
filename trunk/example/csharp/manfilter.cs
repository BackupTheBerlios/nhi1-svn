/**
 *  \file       example/csharp/manfilter.cs
 *  \brief      \$Id: manfilter.cs 507 2009-11-28 15:18:46Z dev1usr $
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
  sealed class manfilter : MqS, IFilterFTR {
    void IFilterFTR.Call () {
      SendSTART();
      while (ReadItemExists()) {
	SendC("<" + ReadC() + ">");
      }
      SendFTR(10);
    }
    static void Main(string[] argv) {
      manfilter srv = new manfilter();
      try {
	srv.ConfigSetName("ManFilter");
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}
