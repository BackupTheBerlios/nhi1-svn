/**
 *  \file       theLink/example/go/src/manfilter/main.go
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

type FTR MqS
  func (this *FTR) Call () {
    ctx := (*MqS)(this)
    ftr := ctx.ServiceGetFilter()
    ftr.SendSTART()
    for ctx.ReadItemExists() {
      ftr.SendC("<" + ctx.ReadC() + ">")
    }
    ftr.SendEND_AND_WAIT2("+FTR")
    ctx.SendRETURN()
  }

func main() {
  srv := NewMqS(nil,nil)
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("ManFilter")
  srv.ConfigSetIsServer(true)
  srv.LinkCreate(os.Args...)
  srv.ServiceCreate("+FTR", (*FTR)(srv))
  srv.ServiceProxy("+EOF")
  srv.ProcessEvent(WAIT_FOREVER)
}
