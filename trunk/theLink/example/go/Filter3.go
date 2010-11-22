/**
 *  \file       theLink/example/go/Filter3.go
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

    MqS IFactory.Factory () {
      return new Filter3();
    }

    void IServerSetup.ServerSetup() {
      MqS ftr = ServiceGetFilter();
      ServiceProxy("+ALL");
      ServiceProxy("+TRT");
      ftr.ServiceProxy("+ALL");
      ftr.ServiceProxy("+TRT");
    }

    public static void Main(string[] argv) {
      Filter3 srv = new Filter3();
      try {
	srv.ConfigSetName("Filter3");
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}



