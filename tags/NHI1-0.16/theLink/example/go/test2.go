/**
 *  \file       theLink/example/go/test2.go
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

import "fmt"

type F func()
type G func (*interface{}, int)

type Fi interface {
  self2(int)
}

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

func (this *second) self2(i int) {
  fmt.Printf("self2 ... *second=%p, i=%d\n", this, i)
}

func misc() {
  println("misc")
}

func DoCall(f Fi, i int) {
  f.self2(i)
}

func main() {
  x := new(second)
  y := F(misc)
  z := F(func() { x.self() })
  z2 := Fi(x)
  x.first.self()
  x.self()
  y()
  z()
  DoCall(x,1)
  z2.self2(2)
}


