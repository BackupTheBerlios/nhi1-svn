/**
 *  \file       theLink/example/go/Filter1.go
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
    //"fmt"
)

type Filter1 struct {
  data [][]string
}

func NewFilter1() *MqS {
  ctx := NewMqS()
  ftr := new(Filter1)
  ctx.ConfigSetFactory(ftr)
  ctx.ConfigSetServerSetup(ftr)
  return ctx
}

func (this *Filter1) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("+FTR", (*FTR)(this))
  ctx.ServiceCreate("+EOF", (*EOF)(this))
}

func (this *Filter1) Factory(ctx *MqS) *MqS {
  return NewFilter1()
}

type FTR Filter1
  func (this *FTR) Call(ctx *MqS) {
    var d []string
    for ctx.ReadItemExists() {
      d = append(d, "<" + ctx.ReadC() + ">")
    }
    this.data = append(this.data, d)
    ctx.SendRETURN()
  }

type EOF Filter1
  func (this *EOF) Call(ctx *MqS) {
    ftr := ctx.ServiceGetFilter2()
    for _,d := range this.data {
      ftr.SendSTART()
      for _,s := range d {
	ftr.SendC(s)
      }
      ftr.SendEND_AND_WAIT2("+FTR")
    }
    ftr.SendSTART()
    ftr.SendEND_AND_WAIT2("+EOF")
    ctx.SendRETURN()
  }

func main() {
  var srv = NewFilter1()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("Filter1")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

