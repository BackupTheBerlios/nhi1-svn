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
  "gomsgque"
  //"reflect"
  "os"
  "fmt"
  //"unsafe"
)

type Server struct {
  *gomsgque.MqS
}

func NewServer() *Server {
  fmt.Println("NewServer...")
  ret := &Server{gomsgque.NewMqS()}
  ret.ConfigSetServerSetup(ret)
  //ret.ConfigSetServerCleanup(ret)
  return ret
}

// //export ServerSetup
func (this *Server) ServerSetup() gomsgque.MqSException {
  var ret gomsgque.MqSException
  return ret
}

func main() {
  var srv = NewServer()
  defer srv.Exit("END")

/*
  if _, ok := interface{}(srv).(gomsgque.IServerSetup); ok {
    fmt.Printf("val = has gomsgque.IServerSetup -> \n")
  }
  if _, ok := interface{}(srv).(gomsgque.IServerCleanup); ok {
    fmt.Printf("val = has gomsgque.IServerCleanup -> \n")
  }
*/

  srv.ConfigSetName("server")
  srv.ConfigSetIdent("test-server")
  if srv.LinkCreate(os.Args...).IsERROR() { return }
  srv.LogC("test",1,"this is the log test\n")
  if srv.ProcessEvent(gomsgque.TIMEOUT_DEFAULT, gomsgque.WAIT_FOREVER).IsERROR() { return }
}

