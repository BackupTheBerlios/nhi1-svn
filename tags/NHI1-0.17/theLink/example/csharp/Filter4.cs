/**
 *  \file       theLink/example/csharp/Filter4.cs
 *  \brief      \$Id: Filter4.cs 92 2009-12-21 11:58:07Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 92 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using System.IO;
using System.Collections.Generic;
using csmsgque;

namespace example {
  sealed class Filter4 : MqS, IServerSetup, IServerCleanup, IEvent, IService {

    Queue<MqDumpS> itms = new Queue<MqDumpS>();
    StreamWriter FH = null;

    public Filter4(MqS tmpl) : base(tmpl) {
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
      Exit();
    }

    void WRIT () {
      Filter4 master = (Filter4) ServiceGetFilter();
      master.FH.WriteLine(ReadC());
      master.FH.Flush();
      SendRETURN();
    }

    void IService.Service (MqS ctx) {
      itms.Enqueue(ReadDUMP());
      SendRETURN();
    }

    void IEvent.Event () {
      if (itms.Count <= 0) {
	ErrorSetCONTINUE();
      } else {
	MqDumpS dump = itms.Peek();
	try  {
	  MqS ftr = ServiceGetFilter();
	  ftr.LinkConnect();
	  ReadForward(ftr, dump);
	} catch (Exception ex) {
	  ErrorSet (ex);
	  if (ErrorIsEXIT()) {
	    ErrorReset();
	    return;
	  } else {
	    ErrorWrite();
	  }
	}
	itms.Dequeue();
      }
    }

    void IServerCleanup.ServerCleanup() {
      if (FH != null) FH.Close();
    }

    void IServerSetup.ServerSetup() {
      Filter4 ftr = (Filter4) ServiceGetFilter();
      ServiceCreate("LOGF", LOGF);
      ServiceCreate("EXIT", EXIT);
      ServiceCreate("+ALL", this);
      ftr.ServiceCreate("WRIT", ftr.WRIT);
      ftr.ServiceProxy("+TRT");
    }

    public static void Main(string[] argv) {
      MqFactoryS<Filter4>.Default("transFilter");
      Filter4 srv = new Filter4(null);
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

