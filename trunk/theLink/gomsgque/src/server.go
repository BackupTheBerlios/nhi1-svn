/**
 *  \file       theLink/gomsgque/src/server.go
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
  "gomsgque"
  "os"
  "fmt"
)

type Server struct {


}

func main() {
  var ctx = gomsgque.NewMqS()
  var val int32
  var ret gomsgque.MqSException
  defer ctx.Exit("END")
  //ctx.ConfigSetName("otto")
  ctx.LogC("client", 0, "START\n")
  ret = ctx.LinkCreate(os.Args...)
    if ret.IsERROR() { return }
  ret = ctx.SendSTART()
    if ret.IsERROR() { return }
  ret = ctx.SendI(100)
    if ret.IsERROR() { return }
  ctx.LogC("client", 0, "SEND\n")
  ret = ctx.SendEND_AND_WAIT("ECOI", gomsgque.TIMEOUT_DEFAULT)
    if ret.IsERROR() { return }
  ctx.LogC("client", 0, "READ\n")
  ret,val = ctx.ReadI()
    if ret.IsERROR() { return }
  ctx.LogC("client", 0, fmt.Sprintf("RESULT = %d\n", val))
  ctx.LogC("client", 0, "END\n")
}

