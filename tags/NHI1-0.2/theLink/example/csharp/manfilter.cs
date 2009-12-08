/**
 *  \file       theLink/example/csharp/manfilter.cs
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
