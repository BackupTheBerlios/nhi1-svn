/**
 *  \file       theLink/example/csharp/Filter5.cs
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
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

// F1 ********************************************************
  sealed class F1 : MqS, IServerSetup {

    public F1(MqS tmpl) : base(tmpl) {
    }

    void FTR () {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F1");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI((int)ftr.ConfigGetStartAs());
      ftr.SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
        ftr.SendC(ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }

    void IServerSetup.ServerSetup () {
      ServiceCreate("+FTR", FTR);
      ServiceProxy("+EOF");
    }
  }

// F2 ********************************************************
  sealed class F2 : MqS, IServerSetup {

    public F2(MqS tmpl) : base(tmpl) {
    }

    void FTR () {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F2");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI((int)ftr.ConfigGetStartAs());
      ftr.SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
        ftr.SendC(ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }

    void IServerSetup.ServerSetup () {
      ServiceCreate("+FTR", FTR);
      ServiceProxy("+EOF");
    }
  }

// F3 ********************************************************
  sealed class F3 : MqS, IServerSetup {

    public F3(MqS tmpl) : base(tmpl) {
    }

    void FTR () {
      MqS ftr = ServiceGetFilter();
      ftr.SendSTART();
      ftr.SendC("F3");
      ftr.SendC(ftr.ConfigGetName());
      ftr.SendI((int)ftr.ConfigGetStartAs());
      ftr.SendI(ConfigGetStatusIs());
      while (ReadGetNumItems() != 0) {
        ftr.SendC(ReadC());
      }
      ftr.SendEND_AND_WAIT("+FTR");
      SendRETURN();
    }

    void IServerSetup.ServerSetup () {
      ServiceCreate("+FTR", FTR);
      ServiceProxy("+EOF");
    }
  }

// Main ******************************************************
  sealed class Filter5 : MqS {
    public static void Main(string[] argv) {
      MqFactoryS<F1>.Add();
      MqFactoryS<F2>.Add();
      MqFactoryS<F3>.Add();
      MqS srv = MqFactoryS<MqS>.GetCalled(argv[0]).New();
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

