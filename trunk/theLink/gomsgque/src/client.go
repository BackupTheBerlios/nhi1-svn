/**
 *  \file       theLink/gomsgque/src/client.go
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

func main() {
  var ctx = NewMqS()
  defer func() {
    if x := recover(); x != nil {
      ctx.ErrorSet(x)
    }
    ctx.Exit()
  }()
  //ctx.ConfigSetName("otto")
  ctx.LogC("client", 0, "START\n")
  ctx.LinkCreate(os.Args...)
  ctx.SendSTART()
  ctx.SendI(100)
  ctx.LogC("client", 0, "SEND\n")
  ctx.SendEND_AND_WAIT("ECOI", MqS_TIMEOUT_DEFAULT)
  ctx.LogC("client", 0, "READ\n")
  ctx.LogC("client", 0, fmt.Sprintf("RESULT = %d\n", ctx.ReadI()))
  ctx.LogC("client", 0, "END\n")
}


