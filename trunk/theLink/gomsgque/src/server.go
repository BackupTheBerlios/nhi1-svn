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

type Server struct {
  *MqS
}

func NewServer() *Server {
  ret := &Server{NewMqS()}
  ret.ConfigSetServerSetup(ret)
  return ret
}

func (this *Server) ServerSetup() {
  this.ServiceCreate("ECOI", &ECOI{this})
  this.ServiceCreate("ECOU", &ECOU{this})
  this.ServiceCreate("ECUL", &ECUL{this})
}

type ECOI struct { *Server }
  func (this *ECOI) Call() {
    this.SendSTART()
    this.SendI(this.ReadI())
    this.SendRETURN()
  }

type ECOU struct { *Server }
  func (this *ECOU) Call() {
    this.SendSTART()
    this.SendU(this.ReadU())
    this.SendRETURN()
  }

type ECUL struct { *Server }
  func (this *ECUL) Call() {
    this.SendSTART()
    this.SendY(this.ReadY())
    this.SendS(this.ReadS())
    this.SendI(this.ReadI())
    this.SendW(this.ReadW())
    this.ReadProxy(this.Server.MqS)
    this.SendRETURN()
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

