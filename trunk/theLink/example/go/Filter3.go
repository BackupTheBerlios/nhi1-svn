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

func NewFilter3() *Filter3 {
  ret := new(Filter3)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *Filter3) Factory() *MqS {
  return NewFilter3().MqS
}

func (this *Filter3) ServerSetup() {
  ftr := this.ServiceGetFilter(0)
  this.ServiceProxy2("+ALL")
  this.ServiceProxy2("+TRT")
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

