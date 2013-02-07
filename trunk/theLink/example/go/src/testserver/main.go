/**
 *  \file       theLink/example/go/src/testserver/main.go
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package main

import (
  . "gomsgque"
    "os"
)

type TestServer struct {
  *MqS
  // add server specific data 
}

func NewTestServer(tmpl *MqS) *MqS {
  srv := new(TestServer)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *TestServer) ServerSetup() {
  this.ServiceCreate("GTCX", (*GTCX)(this))
}

type GTCX TestServer
  func (this *GTCX) Call() {
    this.SendSTART()
    this.SendI(this.LinkGetCtxId())
    this.SendC("+")
    if (this.LinkIsParent()) {
      this.SendI(-1)
    } else {
      this.SendI(this.LinkGetParent().LinkGetCtxId())
    }
    this.SendC("+")
    this.SendC(this.ConfigGetName())
    this.SendC(":")
    this.SendRETURN()
  }

func main() {
  srv := FactoryAdd("TestServer", NewTestServer).New()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

