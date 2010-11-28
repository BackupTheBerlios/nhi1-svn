/**
 *  \file       theLink/gomsgque/src/test1.go
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
//import "unsafe"

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
  h := &g
  fmt.Fprintln(os.Stderr,(string(69))
  fmt.Fprintln(os.Stderr,("g=", g, "h=", h)
  fmt.Fprintln(os.Stderr,(g(1))
  fmt.Fprintln(os.Stderr,(g.call(2))
  fmt.Fprintln(os.Stderr,(DoCall(g,3))
  fmt.Fprintln(os.Stderr,()
  fmt.Fprintln(os.Stderr,((*h)(4))
  fmt.Fprintln(os.Stderr,((*h).call(5))
  fmt.Fprintln(os.Stderr,(DoCall((*h),6))
  return
}

