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
  *MqS
  // add server specific data 
}

func NewFilter3(tmpl *MqS) *MqS {
  srv := new(Filter3)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *Filter3) ServerSetup() {
  ftr := this.ServiceGetFilter()
  this.ServiceProxy("+ALL")
  this.ServiceProxy("+TRT")
  ftr.ServiceProxy("+ALL")
  ftr.ServiceProxy("+TRT")
}

func main() {
  srv := FactoryNew("Filter3", NewFilter3)
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

