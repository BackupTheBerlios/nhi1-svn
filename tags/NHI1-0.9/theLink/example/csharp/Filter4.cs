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
  sealed class Filter4 : MqS, IFactory, IServerSetup, IServerCleanup, IEvent, IService {

    Queue<byte[]> itms = new Queue<byte[]>();
    StreamWriter FH = null;

    MqS IFactory.Factory () {
      return new Filter4();
    }

    void IService.Service (MqS ctx) {
      itms.Enqueue(ReadBDY());
      SendRETURN();
    }

    void IEvent.Event () {
      if (itms.Count <= 0) {
	ErrorSetCONTINUE();
      } else {
	byte[] it = itms.Peek();
	Filter4 ftr = (Filter4) ServiceGetFilter();
	try  {
	  ftr.LinkConnect();
	  ftr.SendBDY(it);
	} catch (Exception ex) {
	  ftr.ErrorSet (ex);
	  if (ftr.ErrorIsEXIT()) {
	    ftr.ErrorReset();
	    return;
	  } else {
	    ftr.ErrorWrite();
	  }
	}
	itms.Dequeue();
      }
    }

    void LOGF () {
      Filter4 ftr = (Filter4) ServiceGetFilter();
      if (ftr.LinkGetTargetIdent() == "transFilter") {
        ftr.SendSTART();
        ftr.SendC(ReadC());
        ftr.SendEND_AND_WAIT("LOGF");
      } else {
	ftr.FH = File.AppendText(ReadC());
      }
      SendRETURN();
    }

    void EXIT () {
      ErrorSetEXIT();
    }

    void WRIT () {
      FH.WriteLine(ReadC());
      FH.Flush();
      SendRETURN();
    }

    void ErrorWrite () {
      FH.WriteLine("ERROR: " + ErrorGetText());
      FH.Flush();
      ErrorReset();
    }

    void IServerCleanup.ServerCleanup() {
      Filter4 ftr = (Filter4)ServiceGetFilter();
      if (ftr.FH != null)
	ftr.FH.Close();
    }

    void IServerSetup.ServerSetup() {
      Filter4 ftr = (Filter4) ServiceGetFilter();
      ServiceCreate("LOGF", LOGF);
      ServiceCreate("EXIT", EXIT);
      ServiceCreate("+ALL", this);
      ftr.ServiceCreate("WRIT", ftr.WRIT);
    }

    public static void Main(string[] argv) {
      Filter4 srv = new Filter4();
      try {
	srv.ConfigSetIgnoreExit(true);
	srv.ConfigSetIdent("transFilter");
	srv.ConfigSetName("Filter4");
	srv.LinkCreate(argv);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}

