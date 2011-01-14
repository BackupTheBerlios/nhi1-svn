/**
 *  \file       theLink/example/csharp/Filter2.cs
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

namespace example {
  sealed class Filter2 : MqS {

    // constructor
    public Filter2(MqS tmpl) : base(tmpl) {
      ConfigSetIsServer(true);
    }

    // service definition from IFilterFTR
    void FTR () {
      throw new ApplicationException("my error");
    }

    public static void Main(String[] argv) {
      Filter2 srv = MqFactoryS<Filter2>.New("filter");
      try {
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
