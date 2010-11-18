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

import . "gomsgque"
import (
  "os"
)

type Server struct {
  *MqS
}

func NewServer() *Server {
  ret := &Server{NewMqS()}
println("NewServer...", ret)
  ret.ConfigSetServerSetup(MqCallback(func() {ret.ServerSetup()}))
  return ret
}

func (this *Server) ServerSetup() {
  println("ServerSetup -> WoW !!!!!!!!!!!!!!!!!!!!!!!!!!")
}

func main() {
  var srv = NewServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  println("srv=", srv)
  srv.ConfigSetName("server")
  srv.ConfigSetIdent("test-server")
  srv.LinkCreate(os.Args...)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(TIMEOUT_DEFAULT, WAIT_FOREVER)
}


