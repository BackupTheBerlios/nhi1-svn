/**
 *  \file       theLink/example/go/Filter6.go
 *  \brief      \$Id: LbMain 304 2010-11-13 09:57:59Z aotto1968 $
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 304 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package main

import (
  . "gomsgque"
    "os"
)

type Filter6 struct {
  *MqS
  FH	*os.File
}

func NewFilter6(tmpl *MqS) *MqS {
  ret := new(Filter6)
  ret.MqS = NewMqS(tmpl, ret)
  return ret.MqS
}

func (this *Filter6) ErrorWrite() {
  this.FH.WriteString("ERROR: " + this.ErrorGetText() + "\n")
  this.ErrorReset()
}

type LOGF Filter6
  func (this *LOGF) Call() {
    ftr := this.ServiceGetFilter()
    if (ftr.LinkGetTargetIdent() == "transFilter") {
      this.ReadForward(ftr)
    } else {
      this.FH,_ = os.Open(this.ReadC(),os.O_WRONLY|os.O_APPEND,0666)
    }
    this.SendRETURN()
  }

type EXIT Filter6
  func (this *EXIT) Call() {
    os.Exit(1)
  }

type WRIT Filter6
  func (this *WRIT) Call() {
    this.ServiceGetFilter().GetSelf().(*Filter6).FH.WriteString(this.ReadC() + "\n")
    this.SendRETURN()
  }

type ALLS Filter6
  func (this *ALLS) Call() {
    this.StorageInsert()
    this.SendRETURN()
  }

func (this *Filter6) Event() {
  if (this.StorageCount() <= 0) {
    this.ErrorSetCONTINUE()
  } else {
    var Id int64 = 0
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
      this.StorageDelete(Id)
    }()
    ftr := this.ServiceGetFilter()
    ftr.LinkConnect()
    Id = this.StorageSelect()
    this.ReadForward(ftr)
  }
}

func (this *Filter6) ServerCleanup() {
  if (this.FH != nil) {
    this.FH.Close()
    this.FH = nil
  }
}

func (this *Filter6) ServerSetup() {
  ftr := this.ServiceGetFilter().GetSelf().(*Filter6)
  this.ServiceCreate("LOGF", (*LOGF)(this))
  this.ServiceCreate("EXIT", (*EXIT)(this))
  this.ServiceCreate("+ALL", (*ALLS)(this))
  this.ServiceStorage("PRNT")
  ftr.ServiceCreate("WRIT", (*WRIT)(ftr))
  ftr.ServiceProxy("+TRT")
}

func main() {
  srv := FactoryAdd("transFilter", NewFilter6).New()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetIgnoreExit(true)
  srv.LinkCreate(os.Args...)
  srv.ProcessEvent(WAIT_FOREVER)
}

