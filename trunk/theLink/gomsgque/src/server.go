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

import "gomsgque"

func main() {
  var ctx = gomsgque.NewMqS()
  ctx.LogC("server", 0, "111\n")
  ctx.LinkCreate()
  ctx.LogC("server", 0, "222\n")
  ctx.ConfigSetName("otto")
  ctx.LogC("server", 0, "333\n")
}

