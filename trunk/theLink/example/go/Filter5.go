/**
 *  \file       theLink/example/go/Filter5.go
 *  \brief      \$Id: Filter5.go 357 2010-12-11 10:36:08Z aotto1968 $
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 357 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package main

import (
  . "gomsgque"
    "os"
)

// F1 ************************************************************

type F1 struct { *MqS }

func F1New (tmpl *MqS) *MqS {
  srv := new(F1)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *F1) ServerSetup() {
  this.ServiceCreate("+FTR", this)
  this.ServiceProxy("+EOF")
}

func (this *F1) Call() {
  ftr := this.ServiceGetFilter()
  ftr.SendSTART()
  ftr.SendC("F1")
  ftr.SendC(ftr.ConfigGetName())
  ftr.SendI(int32(ftr.ConfigGetStartAs()))
  for this.ReadItemExists() {
    ftr.SendC(this.ReadC())
  }
  ftr.SendEND_AND_WAIT2("+FTR")
  this.SendRETURN()
}

// F2 ************************************************************

type F2 struct { *MqS }

func F2New (tmpl *MqS) *MqS {
  srv := new(F2)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *F2) ServerSetup() {
  this.ServiceCreate("+FTR", this)
  this.ServiceProxy("+EOF")
}

func (this *F2) Call() {
  ftr := this.ServiceGetFilter()
  ftr.SendSTART()
  ftr.SendC("F2")
  ftr.SendC(ftr.ConfigGetName())
  ftr.SendI(int32(ftr.ConfigGetStartAs()))
  for this.ReadItemExists() {
    ftr.SendC(this.ReadC())
  }
  ftr.SendEND_AND_WAIT2("+FTR")
  this.SendRETURN()
}

// F3 ************************************************************

type F3 struct { *MqS }

func F3New (tmpl *MqS) *MqS {
  srv := new(F3)
  srv.MqS = NewMqS(tmpl, srv)
  return srv.MqS
}

func (this *F3) ServerSetup() {
  this.ServiceCreate("+FTR", this)
  this.ServiceProxy("+EOF")
}

func (this *F3) Call() {
  ftr := this.ServiceGetFilter()
  ftr.SendSTART()
  ftr.SendC("F3")
  ftr.SendC(ftr.ConfigGetName())
  ftr.SendI(int32(ftr.ConfigGetStartAs()))
  for this.ReadItemExists() {
    ftr.SendC(this.ReadC())
  }
  ftr.SendEND_AND_WAIT2("+FTR")
  this.SendRETURN()
}

// main *********************************************************

func main() {
  FactoryAdd("F1", F1New)
  FactoryAdd("F2", F2New)
  FactoryAdd("F3", F3New)
  srv := FactoryCall(os.Args[1])
  Init(os.Args[0])
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.LinkCreate(os.Args[1:]...)
  srv.ProcessEvent(WAIT_FOREVER)
}

