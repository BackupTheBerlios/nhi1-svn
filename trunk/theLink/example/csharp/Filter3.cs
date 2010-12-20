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
  sealed class Filter3 : MqS, IServerSetup {

    public Filter3(MqS tmpl) : base(tmpl) {
    }

    void IServerSetup.ServerSetup() {
      MqS ftr = ServiceGetFilter();
      ServiceProxy("+ALL");
      ServiceProxy("+TRT");
      ftr.ServiceProxy("+ALL");
      ftr.ServiceProxy("+TRT");
    }

    public static void Main(string[] argv) {
      Filter3 srv = MqFactoryS<Filter3>.New();
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



