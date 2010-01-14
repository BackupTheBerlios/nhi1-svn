/**
 *  \file       theLink/example/csharp/MyServer.cs
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

  sealed class MyServer : MqS, IServerSetup, IFactory {

    // create new intances
    MqS IFactory.Factory() {
      return new MyServer();
    }

    // service to serve all incomming requests for token "HLWO"
    public void MyFirstService () {
      SendSTART();
      SendC("Hello World");
      SendRETURN();
    }

    // define a service as link between the token "HLWO" and the callback "MyFirstService"
    void IServerSetup.ServerSetup() {
      ServiceCreate("HLWO", MyFirstService);
    }

    static void Main(string[] argv) {
      MyServer srv = new MyServer();
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
