/**
 *  \file       theLink/example/go/src/manfilter.go
 *  \brief      \$Id: manfilter.go 320 2010-11-21 17:05:19Z aotto1968 $
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
  sealed class manfilter : MqS, IFactory {
    MqS IFactory.Factory () {
      return new manfilter();
    }
    void FTR () {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      while (ReadItemExists()) {
	ftr.SendC("<" + ReadC() + ">");
      }
      ftr.SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }
    static void Main(string[] argv) {
      manfilter srv = new manfilter();
      try {
	srv.ConfigSetName("ManFilter");
	srv.ConfigSetIsServer(true);
	srv.LinkCreate(argv);
	srv.ServiceCreate("+FTR", srv.FTR);
	srv.ServiceProxy ("+EOF");
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}

