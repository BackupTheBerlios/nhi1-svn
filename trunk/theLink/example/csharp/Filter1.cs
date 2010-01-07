/**
 *  \file       theLink/example/csharp/Filter1.cs
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
  sealed class Filter1 : MqS, IFactory {

    private List<List<string>> data = new List<List<string>>();

    MqS IFactory.Call () {
      return new Filter1();
    }

    // service definition
    void FTR () {
      List<string> d = new List<string>();
      while (ReadItemExists()) {
	d.Add("<" + ReadC() + ">");
      }
      data.Add(d);
      SendRETURN();
    }

    // service definition
    void EOF () {
      MqS ftr = ServiceGetFilter();
      foreach (List<string> d in data) {
	ftr.SendSTART();
	foreach (string s in d) {
	  ftr.SendC(s);
	}
	ftr.SendEND_AND_WAIT("+FTR");
      }
      ftr.SendSTART();
      ftr.SendEND_AND_WAIT("+EOF");
      SendRETURN();
    }

    public static void Main(string[] argv) {
      Filter1 srv = new Filter1();
      try {
	srv.ConfigSetName("filter");
	srv.ConfigSetIsServer(true);
	srv.LinkCreate(argv);
	srv.ServiceCreate("+FTR", srv.FTR);
	srv.ServiceCreate("+EOF", srv.EOF);
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}

