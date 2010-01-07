/**
 *  \file       theLink/example/csharp/Filter3.cs
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
using System.Collections.Generic;

namespace example {
  sealed class Filter3 : MqS, IFactory, IServerSetup {

    MqS IFactory.Call () {
      return new Filter3();
    }

    // service definition
    void Filter () {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendBDY(ReadBDY());
      if (ServiceIsTransaction()) {
	ftr.SendEND_AND_WAIT(ServiceGetToken());
	SendSTART();
	SendBDY(ftr.ReadBDY());
      } else {
	ftr.SendEND(ServiceGetToken());
      }
      SendRETURN();
    }

    void IServerSetup.Call() {
      MqS ftr = ServiceGetFilter();
      ServiceCreate("+ALL", Filter);
      ftr.ServiceCreate("+ALL", Filter);
    }

    public static void Main(string[] argv) {
      Filter3 srv = new Filter3();
      try {
	srv.ConfigSetName("filter");
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}



