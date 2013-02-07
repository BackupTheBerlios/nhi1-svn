/**
 *  \file       theLink/example/go/src/client/main.go
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
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
  "fmt"
)

func Call(ctx *MqS) {
  ctx.SendSTART()
  ctx.SendI(100)
  ctx.SendEND_AND_WAIT("ECOI", TIMEOUT_DEFAULT)
  ctx.LogC("client", 0, fmt.Sprintf("RESULT = %d\n", ctx.ReadI()))
}

func main() {
  var ctx1 = NewMqS(nil)
  var ctx2 = NewMqS(nil)
  var ctx3 = NewMqS(nil)
  defer func() {
    if x := recover(); x != nil {
      ctx1.ErrorSet(x)
    }
    ctx1.Exit()
  }()
  ctx1.LinkCreate(os.Args...)
  ctx2.LinkCreate(os.Args...)
  ctx3.LinkCreate(os.Args...)
  Call(ctx1)
  Call(ctx2)
  Call(ctx3)
}

