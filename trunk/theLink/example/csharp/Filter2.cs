/**
 *  \file       theLink/example/csharp/Filter2.cs
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {
  sealed class Filter2 : MqS, IFilterFTR {

    // service definition from IFilterFTR
    void IFilterFTR.Call () {
      throw new ApplicationException("my error");
    }

    public static void Main(String[] argv) {
      Filter2 srv = new Filter2();
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
