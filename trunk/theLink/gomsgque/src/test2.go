/**
 *  \file       theLink/gomsgque/src/test2.go
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

type F func()
type G func (*second) ()

type first struct {
}

func (this *first) self() {
  println("*first=", this)
}

type second struct {
  first
}

func (this *second) self() {
  println("*second=", this)
}

func misc() {
  println("misc")
}

func main() {
  x := new(second)
  y := F(misc)
  z := F(func() { x.self() })
  x.first.self()
  x.self()
  y()
  z()
}

