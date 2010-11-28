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
  *MqS
  // add server specific data 
}

func NewMyServer() *MyServer {
  ret := new(MyServer)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *MyServer) Factory() *MqS {
  return NewMyServer().MqS
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
  var srv = NewMyServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

