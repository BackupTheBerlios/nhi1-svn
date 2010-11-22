/**
 *  \file       theLink/example/go/src/Filter2.go
 *  \brief      \$Id: Filter2.go 320 2010-11-21 17:05:19Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 320 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
using System;
using csmsgque;

namespace example {
  sealed class Filter2 : MqS, IFactory {

    MqS IFactory.Factory () {
      return new Filter2();
    }

    // service definition from IFilterFTR
    void FTR () {
      throw new ApplicationException("my error");
    }

    public static void Main(String[] argv) {
      Filter2 srv = new Filter2();
      try {
	srv.ConfigSetName("filter");
	srv.ConfigSetIsServer(true);
	srv.LinkCreate(argv); 
	srv.ServiceCreate("+FTR", srv.FTR);
	srv.ServiceProxy ("+EOF");
	srv.ProcessEvent(MqS.WAIT.FOREVER);
      } catch (Exception ex) {
        srv.ErrorSet (ex);
      }
      srv.Exit();
    }
  }
}
