/**
 *  \file       theLink/example/go/Filter3.go
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

type Filter3 struct {
  // add server specific data 
}

func NewFilter3() *MqS {
  return NewMqS(new(Filter3))
}

func (this *Filter3) Factory(ctx *MqS) *MqS {
  return NewFilter3()
}

func (this *Filter3) ServerSetup(ctx *MqS) {
  ftr := ctx.ServiceGetFilter2()
  ctx.ServiceProxy2("+ALL")
  ctx.ServiceProxy2("+TRT")
  ftr.ServiceProxy2("+ALL")
  ftr.ServiceProxy2("+TRT")
}

func main() {
  var srv = NewFilter3()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("Filter3")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

