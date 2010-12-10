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
  *MqS
  data [][]string
}

func NewFilter1(tmpl *MqS) *MqS {
  return NewMqS(tmpl, new(Filter1))
}

func (this *Filter1) ServerSetup() {
  this.ServiceCreate("+FTR", (*FTR)(this))
  this.ServiceCreate("+EOF", (*EOF)(this))
}

type FTR Filter1
  func (this *FTR) Call() {
    var d []string
    for this.ReadItemExists() {
      d = append(d, "<" + this.ReadC() + ">")
    }
    this.data = append(this.data, d)
    this.SendRETURN()
  }

type EOF Filter1
  func (this *EOF) Call() {
    ftr := this.ServiceGetFilter()
    for _,d := range this.data {
      ftr.SendSTART()
      for _,s := range d {
	ftr.SendC(s)
      }
      ftr.SendEND_AND_WAIT2("+FTR")
    }
    ftr.SendSTART()
    ftr.SendEND_AND_WAIT2("+EOF")
    this.SendRETURN()
  }

func main() {
  srv := FactoryNew("Filter1", NewFilter1)
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

