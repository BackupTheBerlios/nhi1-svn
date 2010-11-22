/**
 *  \file       theLink/example/go/manfilter.go
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

type ManFilter MqS
  func (this *ManFilter) Factory (ctx *MqS) *MqS {
    return NewMqS()
  }

type FTR MqS
  func (this *FTR) Call (ctx *MqS) {
    ftr := ctx.ServiceGetFilter2()
    ftr.SendSTART()
    for ctx.ReadItemExists() {
      ftr.SendC("<" + ctx.ReadC() + ">")
    }
    ftr.SendEND_AND_WAIT2("+FTR")
    ctx.SendRETURN()
  }

func main() {
  var srv = NewMqS()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("ManFilter")
  srv.ConfigSetIsServer(true)
  srv.ConfigSetFactory((*ManFilter)(srv))
  srv.LinkCreate(os.Args...)
  srv.ServiceCreate("+FTR", (*FTR)(srv))
  srv.ServiceProxy2("+EOF")
  srv.ProcessEvent2(MqS_WAIT_FOREVER)
}

