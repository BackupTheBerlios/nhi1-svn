/**
 *  \file       theLink/example/go/Filter2.go
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
    //"fmt"
)

type Filter2 struct {
  // add server specific data 
}

func NewFilter2() *MqS {
  ctx := NewMqS()
  ftr := new(Filter2)
  ctx.ConfigSetFactory(ftr)
  ctx.ConfigSetServerSetup(ftr)
  return ctx
}

func (this *Filter2) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("+FTR", (*FTR)(this))
  ctx.ServiceProxy2("+EOF")
}

func (this *Filter2) Factory(ctx *MqS) *MqS {
  return NewFilter2()
}

type FTR Filter2
  func (this *FTR) Call(ctx *MqS) {
    panic("my error")
  }

func main() {
  var srv = NewFilter2()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("filter")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

