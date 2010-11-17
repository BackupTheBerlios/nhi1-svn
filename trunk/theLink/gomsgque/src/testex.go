/**
 *  \file       theLink/gomsgque/src/testex.go
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
import "unsafe"

type F func(int) string
type Fi interface {
  call(int) string
}

func f(i int) string {
  print("the function -> ")
  return fmt.Sprintf("i=%d -> ",i)
}

func (f F) call(i int) string {
  print("indirectly -> ")
  return f(i)
}

func DoCall(f Fi, i int) string {
  print("double -> ");
  return f.call(i)
}

func main() {
  g := F(f)
  h := unsafe.Pointer(&g)
  println(g, &g, h)
  println(g(1))
  println(g.call(2))
  println(DoCall(g,3))
  println()
  //println(h(4))
  //println(h.call(5))
  //println(DoCall(h,6))
  return
}
