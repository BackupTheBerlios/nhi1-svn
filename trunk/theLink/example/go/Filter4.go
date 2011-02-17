/**
 *  \file       theLink/example/go/Filter4.go
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
    "container/list"
    "os"
)

type Filter4 struct {
  *MqS
  itms	*list.List
  FH	*os.File
}

func NewFilter4(tmpl *MqS) *MqS {
  ret := new(Filter4)
  ret.MqS = NewMqS(tmpl, ret)
  ret.itms = list.New()
  return ret.MqS
}

func (this *Filter4) ErrorWrite() {
  this.FH.WriteString("ERROR: " + this.ErrorGetText() + "\n")
  this.ErrorReset()
}

type LOGF Filter4
  func (this *LOGF) Call() {
    ftr := this.ServiceGetFilter()
    if (ftr.LinkGetTargetIdent() == "transFilter") {
      this.ReadForward(ftr)
    } else {
      this.FH,_ = os.Open(this.ReadC(),os.O_WRONLY|os.O_APPEND,0666)
    }
    this.SendRETURN()
  }

type EXIT Filter4
  func (this *EXIT) Call() {
    os.Exit(1)
  }

type WRIT Filter4
  func (this *WRIT) Call() {
    this.ServiceGetFilter().GetSelf().(*Filter4).FH.WriteString(this.ReadC() + "\n")
    this.SendRETURN()
  }

type ALLS Filter4
  func (this *ALLS) Call() {
    this.itms.PushBack(this.ReadDUMP())
    this.SendRETURN()
  }

func (this *Filter4) Event() {
  if (this.itms.Len() <= 0) {
    this.ErrorSetCONTINUE()
  } else {
    it := this.itms.Front()
    defer func() {
      if x := recover(); x != nil {
	this.ErrorSet(x)
	if this.ErrorIsEXIT() {
	  this.ErrorReset()
	  return;
	} else {
	  this.ErrorWrite()
	}
      }
      this.itms.Remove(it)
    }()
    ftr := this.ServiceGetFilter()
    ftr.LinkConnect()
    this.ReadLOAD(it.Value.(*MqDumpS))
    this.ReadForward(ftr)
  }
}

func (this *Filter4) ServerCleanup() {
  if (this.FH != nil) {
    this.FH.Close()
    this.FH = nil
  }
}

func (this *Filter4) ServerSetup() {
  ftr := this.ServiceGetFilter().GetSelf().(*Filter4)
  this.ServiceCreate("LOGF", (*LOGF)(this))
  this.ServiceCreate("EXIT", (*EXIT)(this))
  this.ServiceCreate("+ALL", (*ALLS)(this))
  ftr.ServiceCreate("WRIT", (*WRIT)(ftr))
  ftr.ServiceProxy("+TRT")
}

func main() {
  FactoryDefault("transFilter", NewFilter4)
  srv := NewFilter4(nil)
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetIgnoreExit(true)
  srv.ConfigSetName("Filter4")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

