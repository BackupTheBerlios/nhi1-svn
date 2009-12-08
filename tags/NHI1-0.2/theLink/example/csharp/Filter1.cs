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
  sealed class Filter1 : MqS, IFilterFTR, IFilterEOF {

    private List<List<string>> data = new List<List<string>>();

    // service definition
    void IFilterFTR.Call () {
      List<string> d = new List<string>();
      while (ReadItemExists()) {
	d.Add("<" + ReadC() + ">");
      }
      data.Add(d);
    }

    // service definition
    void IFilterEOF.Call () {
      foreach (List<string> d in data) {
	SendSTART();
	foreach (string s in d) {
	  SendC(s);
	}
	SendFTR(10);
      }
    }

    public static void Main(string[] argv) {
      Filter1 srv = new Filter1();
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
