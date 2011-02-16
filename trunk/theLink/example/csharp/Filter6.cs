/**
 *  \file       theLink/example/csharp/Filter6.cs
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using System.IO;
using System.Collections.Generic;
using csmsgque;

namespace example {
  sealed class Filter6 : MqS, IServerSetup, IServerCleanup, IEvent, IService {

    StreamWriter FH = null;

    public Filter6(MqS tmpl) : base(tmpl) {
    }

    void ErrorWrite () {
      FH.WriteLine("ERROR: " + ErrorGetText());
      FH.Flush();
      ErrorReset();
    }

    void LOGF () {
      MqS ftr = ServiceGetFilter();
      if (ftr.LinkGetTargetIdent() == "transFilter") {
	ReadForward(ftr);
      } else {
	FH = File.AppendText(ReadC());
      }
      SendRETURN();
    }

    void EXIT () {
      Environment.Exit (1);
    }

    void WRIT () {
      Filter6 master = (Filter6) ServiceGetFilter();
      master.FH.WriteLine(ReadC());
      master.FH.Flush();
      SendRETURN();
    }

    void IService.Service (MqS ctx) {
      StorageInsert();
      SendRETURN();
    }

    void IEvent.Event () {
      if (StorageCount() == 0) {
	ErrorSetCONTINUE();
      } else {
	long Id = 0L;
	try  {
	  MqS ftr = ServiceGetFilter();
	  ftr.LinkConnect();
	  Id = StorageSelect();
	  ReadForward(ftr);
	} catch (Exception ex) {
	  ErrorSet (ex);
	  if (ErrorIsEXIT()) {
	    ErrorReset();
	    return;
	  } else {
	    ErrorWrite();
	  }
	}
	StorageDelete(Id);
      }
    }

    void IServerCleanup.ServerCleanup() {
      if (FH != null) FH.Close();
    }

    void IServerSetup.ServerSetup() {
      Filter6 ftr = (Filter6) ServiceGetFilter();
      ServiceCreate("LOGF", LOGF);
      ServiceCreate("EXIT", EXIT);
      ServiceCreate("+ALL", this);
      ServiceStorage("PRNT");
      ftr.ServiceCreate("WRIT", ftr.WRIT);
      ftr.ServiceProxy("+TRT");
    }

    public static void Main(string[] argv) {
      Filter6 srv = MqFactoryS<Filter6>.Add("transFilter").New();
      try {
	srv.ConfigSetIgnoreExit(true);
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}

