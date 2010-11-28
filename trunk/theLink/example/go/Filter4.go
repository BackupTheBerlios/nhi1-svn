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

func NewFilter4() *Filter4 {
  ret := new(Filter4)
  ret.MqS = NewMqS(ret)
  ret.itms = list.New()
  return ret
}

func (this *Filter4) ServerCleanup() {
  ftr := this.ServiceGetFilter().GetSelf().(*Filter4)
  if (ftr.FH != nil) {
    ftr.FH.Close()
    ftr.FH = nil
  }
}

func (this *Filter4) ServerSetup() {
  ftr := this.ServiceGetFilter().GetSelf().(*Filter4)
  this.ServiceCreate("LOGF", (*LOGF)(this))
  this.ServiceCreate("EXIT", (*EXIT)(this))
  this.ServiceCreate("+ALL", (*ALLS)(this))
  ftr.ServiceCreate("WRIT", (*WRIT)(ftr))
}

func (this *Filter4) Factory() *MqS {
  return NewFilter4().MqS
}

type ALLS Filter4
  func (this *ALLS) Call() {
    this.itms.PushBack(this.ReadBDY().Get())
    this.SendRETURN()
  }

func (this *Filter4) Event() {
  if (this.itms.Len() <= 0) {
    this.ErrorSetCONTINUE()
  } else {
    it := this.itms.Front()
    ftr := this.ServiceGetFilter().GetSelf().(*Filter4)
    defer func() {
      if x := recover(); x != nil {
	ftr.ErrorSet(x)
	if ftr.ErrorIsEXIT() {
	  ftr.ErrorReset()
	  return;
	} else {
	  ftr.ErrorWrite()
	}
      }
      this.itms.Remove(it)
    }()
    ftr.LinkConnect()
    ftr.SendBDY(new(MqBinary).Set(it.Value.([]byte)))
  }
}

type LOGF Filter4
  func (this *LOGF) Call() {
    ftr := this.ServiceGetFilter().GetSelf().(*Filter4)
    if (ftr.LinkGetTargetIdent() == "transFilter") {
      ftr.SendSTART()
      ftr.SendC(this.ReadC())
      ftr.SendEND_AND_WAIT2("LOGF")
    } else {
      ftr.FH,_ = os.Open(this.ReadC(),os.O_WRONLY|os.O_APPEND,0666)
    }
    this.SendRETURN()
  }

type EXIT Filter4
  func (this *EXIT) Call() {
    this.ErrorSetEXIT()
  }

type WRIT Filter4
  func (this *WRIT) Call() {
    this.FH.WriteString(this.ReadC() + "\n")
    this.SendRETURN()
  }

func (this *Filter4) ErrorWrite() {
  this.FH.WriteString("ERROR: " + this.ErrorGetText() + "\n")
  this.ErrorReset()
}

func main() {
  var srv = NewFilter4()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetIgnoreExit(true)
  srv.ConfigSetIdent("transFilter")
  srv.ConfigSetName("Filter4")
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

