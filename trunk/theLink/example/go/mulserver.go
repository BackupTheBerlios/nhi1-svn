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
  // add server specific data 
}

func Newmulserver() *MqS {
  return NewMqS(new(mulserver))
}

func (this *mulserver) Factory(ctx *MqS) *MqS {
  return Newmulserver()
}

func (this *mulserver) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("MMUL", (*MMUL)(this))
}

type MMUL mulserver
  func (this *MMUL) Call(ctx *MqS) {
    ctx.SendSTART();
    ctx.SendD(ctx.ReadD() * ctx.ReadD());
    ctx.SendRETURN();
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
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

