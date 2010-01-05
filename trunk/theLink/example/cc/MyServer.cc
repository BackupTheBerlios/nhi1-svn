/**
 *  \file       theLink/example/cc/MyServer.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "ccmsgque.h"

using namespace ccmsgque;

class MyServer : public MqC, public IServerSetup, public IFactory {

  // service to serve all incomming requests for token "HLWO"
  void MyFirstService () {
    SendSTART();
    SendC("Hello World");
    SendRETURN();
  }

  // factory to create objects
  MqC* Factory() const {return new MyServer();}

  // define a service as link between the token "HLWO" and the callback "MyFirstService"
  void ServerSetup() {
    ServiceCreate("HLWO", CallbackF(&MyServer::MyFirstService));
  }
};

int MQ_CDECL main(int argc, MQ_CST argv[]) {
  MyServer srv;
  try {
    srv.LinkCreateVC(argc, argv);
    srv.ProcessEvent (MQ_WAIT_FOREVER);
  } catch (const exception& e) {
    srv.ErrorSet(e);
  }
  srv.Exit();
}
