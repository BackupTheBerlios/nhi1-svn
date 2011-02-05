/**
 *  \file       theLink/example/go/mulclient.go
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
    "fmt"
)

func main() {
  var ctx = NewMqS(nil,nil)
  defer func() {
    if x := recover(); x != nil {
      ctx.ErrorSet(x)
    }
    ctx.Exit()
  }()
  ctx.ConfigSetName("MyMul")
  ctx.LinkCreate(os.Args...)
  ctx.SendSTART()
  ctx.SendD(3.67)
  ctx.SendD(22.3)
  ctx.SendEND_AND_WAIT("MMUL", 5)
  fmt.Printf("%f", ctx.ReadD())
}
