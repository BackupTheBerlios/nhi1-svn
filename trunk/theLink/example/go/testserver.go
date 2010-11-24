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
  // add server specific data 
}

func NewTestServer() *MqS {
  return NewMqS(new(TestServer))
}

func (this *TestServer) Factory(ctx *MqS) *MqS {
  return NewTestServer()
}

func (this *TestServer) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("GTCX", (*GTCX)(this))
}

type GTCX TestServer
  func (this *GTCX) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendI(ctx.LinkGetCtxId())
    ctx.SendC("+")
    if (ctx.LinkIsParent()) {
      ctx.SendI(-1)
    } else {
      ctx.SendI(ctx.LinkGetParent().LinkGetCtxId())
    }
    ctx.SendC("+")
    ctx.SendC(ctx.ConfigGetName())
    ctx.SendC(":")
    ctx.SendRETURN()
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

