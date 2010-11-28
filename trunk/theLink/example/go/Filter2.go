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
  *MqS
  // add server specific data 
}

func NewFilter2() *Filter2 {
  ret := new(Filter2)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *Filter2) ServerSetup() {
  this.ServiceCreate("+FTR", (*FTR)(this))
  this.ServiceProxy("+EOF")
}

func (this *Filter2) Factory() *MqS {
  return NewFilter2().MqS
}

type FTR Filter2
  func (this *FTR) Call() {
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
  srv.ProcessEvent(WAIT_FOREVER)
}

