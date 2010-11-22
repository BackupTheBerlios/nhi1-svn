/**
 *  \file       theLink/example/go/src/MyClient.go
 *  \brief      \$Id: MyClient.go 320 2010-11-21 17:05:19Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 320 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package main

import (
  . "gomsgque"
  "os"
)

func main() {
  var ctx = NewMqS()
  defer func() {
    if x := recover(); x != nil {
      ctx.ErrorSet(x)
    }
    ctx.Exit()
  }()
  ctx.LinkCreate(os.Args...)
  ctx.SendSTART()
  ctx.SendI(100)
  ctx.SendEND_AND_WAIT("HLWO", 5)
  println(ctx.ReadC())
}

