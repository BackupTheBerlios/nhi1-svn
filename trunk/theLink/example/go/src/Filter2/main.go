/**
 *  \file       theLink/example/go/src/Filter2/main.go
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
  *MqS
  // add server specific data 
}

func NewFilter2(tmpl *MqS) *MqS {
  srv := new(Filter2)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *Filter2) ServerSetup() {
  this.ServiceCreate("+FTR", (*FTR)(this))
  this.ServiceProxy("+EOF")
}

type FTR Filter2
  func (this *FTR) Call() {
    panic("my error")
  }

func main() {
  srv := FactoryAdd("filter", NewFilter2).New()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

