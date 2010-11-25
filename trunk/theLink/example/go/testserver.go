/**
 *  \file       theLink/example/go/testserver.go
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

func NewTestServer() *TestServer {
  ret := new(TestServer)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *TestServer) Factory() *MqS {
  return NewTestServer().MqS
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
  var srv = NewTestServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

