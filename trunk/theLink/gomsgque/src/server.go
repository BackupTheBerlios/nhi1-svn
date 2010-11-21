/**
 *  \file       theLink/gomsgque/src/server.go
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
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

/*
type Server struct {
  MqS
}

func NewServer() *Server {
  ret := new(Server)
  ret.Init()
  ret.ConfigSetServerSetup(ret)
  return ret
}
*/

type Server struct {
  *MqS
}

func NewServer() *Server {
  ret := &Server{NewMqS()}
  ret.ConfigSetServerSetup(ret)
  return ret
}

func (this *Server) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("ECOI", (*ECOI)(this))
  ctx.ServiceCreate("ECOU", (*ECOU)(this))
  ctx.ServiceCreate("ECUL", (*ECUL)(this))
}

type ECOI Server
  func (this *ECOI) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendI(ctx.ReadI())
    ctx.SendRETURN()
  }

type ECOU Server
  func (this *ECOU) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendU(ctx.ReadU())
    ctx.SendRETURN()
  }

type ECUL Server
  func (this *ECUL) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendY(ctx.ReadY())
    ctx.SendS(ctx.ReadS())
    ctx.SendI(ctx.ReadI())
    ctx.SendW(ctx.ReadW())
    ctx.ReadProxy(ctx)
    ctx.SendRETURN()
    //ctx.ErrorC("ECUL",-1,"fehler")
  }

func main() {
  var srv = NewServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("server")
  srv.ConfigSetIdent("test-server")
  srv.LinkCreate(os.Args...)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS_TIMEOUT_DEFAULT, MqS_WAIT_FOREVER)
}

