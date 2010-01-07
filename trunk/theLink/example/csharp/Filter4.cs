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
using csmsgque;
using System.Collections.Generic;

namespace example {
  sealed class Filter4 : MqS, IFactory, IServerSetup, IEvent, IService {

    struct FilterItmS {
      public string token;
      public bool isTransaction;
      public byte[] bdy;
    }
    Queue<FilterItmS> itms = new Queue<FilterItmS>();

    MqS IFactory.Call () {
      return new Filter4();
    }

    void IService.Call (MqS ctx) {
      FilterItmS it;
      it.token = ServiceGetToken();
      it.isTransaction = ServiceIsTransaction();
      it.bdy = ReadBDY();
      itms.Enqueue(it);
      SendRETURN();
    }

    void IEvent.Call () {
      if (itms.Count > 0) {
	FilterItmS it = itms.Peek();
	try  {
	  MqS ftr = ServiceGetFilter();
	  ftr.SendSTART();
	  ftr.SendBDY(it.bdy);
	  if (it.isTransaction) {
	    ftr.SendEND_AND_WAIT(it.token);
	  } else {
	    ftr.SendEND(it.token);
	  }
	} catch (Exception ex) {
	  ErrorSet (ex);
	  ErrorPrint();
	  ErrorReset();
	} finally {
	  itms.Dequeue();
	}
      } else {
	ErrorSetCONTINUE();
      }
    }

    void IServerSetup.Call() {
      ServiceCreate("+ALL", this);
    }

    public static void Main(string[] argv) {
      Filter4 srv = new Filter4();
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

