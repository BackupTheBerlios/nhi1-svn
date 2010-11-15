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
  "MqS"
  "os"
  "fmt"
)

func main() {
  var ctx = MqS.NewMqS()
  var val int32
  ret := uint32(MqS.OK)
  defer ctx.Exit("END")
  //ctx.ConfigSetName("otto")
  ctx.LogC("server", 0, "START\n")
  ret = ctx.LinkCreate(os.Args...)
    if ret == MqS.ERROR { return }
  ret = ctx.SendSTART()
    if ret == MqS.ERROR { return }
  ret = ctx.SendI(100)
    if ret == MqS.ERROR { return }
  ctx.LogC("server", 0, "SEND\n")
  ret = ctx.SendEND_AND_WAIT("ECOI", MqS.TIMEOUT_DEFAULT)
    if ret == MqS.ERROR { return }
  ctx.LogC("server", 0, "READ\n")
  ret,val = ctx.ReadI()
    if ret == MqS.ERROR { return }
  ctx.LogC("server", 0, fmt.Sprintf("RESULT = %d\n", val))
  ctx.LogC("server", 0, "END\n")
}

