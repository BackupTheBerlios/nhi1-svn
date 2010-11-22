/**
 *  \file       theLink/example/go/MyServer.go
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

type MyServer struct {
  // add server specific data 
}

func NewMyServer() *MqS {
  ctx := NewMqS()
  srv := new(MyServer)
  ctx.ConfigSetServerSetup(srv)
  ctx.ConfigSetFactory(srv)
  return ctx
}

func (this *MyServer) Factory(ctx *MqS) *MqS {
  return NewMyServer()
}

func (this *MyServer) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("HLWO", (*MyFirstService)(this))
}

type MyFirstService MyServer
  func (this *MyFirstService) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendC("Hello World")
    ctx.SendRETURN()
  }

func main() {
  var srv = NewMyServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(MqS_TIMEOUT_DEFAULT, MqS_WAIT_FOREVER)
}

