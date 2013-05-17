/**
 *  \file       theLink/example/go/src/MyServer/main.go
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
  *MqS
  // add server specific data 
}

func NewMyServer(tmpl *MqS) *MqS {
  srv := new(MyServer)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *MyServer) ServerSetup() {
  this.ServiceCreate("HLWO", (*MyFirstService)(this))
}

type MyFirstService MyServer
  func (this *MyFirstService) Call() {
    this.SendSTART()
    this.SendC("Hello World")
    this.SendRETURN()
  }

func main() {
  srv := FactoryAdd("MyServer", NewMyServer).New()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

