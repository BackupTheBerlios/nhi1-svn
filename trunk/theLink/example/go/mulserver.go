/**
 *  \file       theLink/example/go/mulserver.go
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

type mulserver struct {
  *MqS
  // add server specific data 
}

func Newmulserver() *mulserver {
  ret := new(mulserver)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *mulserver) Factory() *MqS {
  return Newmulserver().MqS
}

func (this *mulserver) ServerSetup() {
  this.ServiceCreate("MMUL", (*MMUL)(this))
}

type MMUL mulserver
  func (this *MMUL) Call() {
    this.SendSTART();
    this.SendD(this.ReadD() * this.ReadD());
    this.SendRETURN();
  }

func main() {
  var srv = Newmulserver()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("MyMulServer")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

