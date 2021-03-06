/**
 *  \file       theLink/example/go/testclient.go
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
    "path"
    "fmt"
)

func Get(ctx *MqS) string {
  var RET string
  ctx.SendSTART()
  ctx.SendEND_AND_WAIT("GTCX", -1)
  RET += ctx.ConfigGetName()
  RET += "+"
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  return RET
}

func main() {
  // create the object
  c0	:= NewMqS(nil,nil)
  c00	:= NewMqS(nil,nil)
  c01	:= NewMqS(nil,nil)
  c000	:= NewMqS(nil,nil)
  c1	:= NewMqS(nil,nil)
  c10	:= NewMqS(nil,nil)
  c100	:= NewMqS(nil,nil)
  c101	:= NewMqS(nil,nil)

  // catch errors and exit the app
  defer func() {
    if x := recover(); x != nil {
      c0.ErrorSet(x)
    }
    c0.Exit();
  }()

  // get server
  dir, _ := path.Split(os.Args[0])
  server := path.Join(dir, "testserver")
  LIST := append([]string{os.Args[0], "--name", "c1", "--srvname", "s1"}, os.Args[1:]...)

  // create the link
  c0.LinkCreate(os.Args[0], "--name", "c0", "--debug", os.Getenv("TS_DEBUG"), "@", server, "--silent", "--name", "s0")
  c00.LinkCreateChild(c0, os.Args[0], "--name", "c00", "--srvname", "s00")
  c01.LinkCreateChild(c0, os.Args[0], "--name", "c01", "--srvname", "s01")
  c000.LinkCreateChild(c00, os.Args[0], "--name", "c000", "--srvname", "s000")
  c1.LinkCreate(LIST...)
  c10.LinkCreateChild(c1, os.Args[0], "--name", "c10", "--srvname", "s10")
  c100.LinkCreateChild(c10, os.Args[0], "--name", "c100", "--srvname", "s100")
  c101.LinkCreateChild(c10, os.Args[0], "--name", "c101", "--srvname", "s101")

  // do the tests
  fmt.Fprintln(os.Stdout,Get(c0))
  fmt.Fprintln(os.Stdout,Get(c00))
  fmt.Fprintln(os.Stdout,Get(c01))
  fmt.Fprintln(os.Stdout,Get(c000))
  fmt.Fprintln(os.Stdout,Get(c1))
  fmt.Fprintln(os.Stdout,Get(c10))
  fmt.Fprintln(os.Stdout,Get(c100))
  fmt.Fprintln(os.Stdout,Get(c101))

  // cleanup
  c0.LinkDelete()
  c1.LinkDelete()
}

