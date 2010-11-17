/**
 *  \file       theLink/gomsgque/src/MqS.go
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package gomsgque

/*
#include <gomsgque.h>
*/
import "C"

import (
  //"fmt"
  //"strings"
  "unsafe"
)

type TIMEOUT  C.MQ_TIME_T
type WAIT     uint32

const (
  OK		  MqSException  = 0
  CONTINUE	  MqSException  = 1
  ERROR		  MqSException  = 2

  TIMEOUT_DEFAULT TIMEOUT = -1
  TIMEOUT_USER	  TIMEOUT = -2
  TIMEOUT_MAX	  TIMEOUT = -3

  WAIT_NO	  WAIT	= 0
  WAIT_ONCE	  WAIT	= 1
  WAIT_FOREVER	  WAIT	= 2
)

type MqS _Ctype_struct_MqS

type IServerSetup interface {
  ServerSetup() MqSException
}

type IServerCleanup interface {
  ServerCleanup() MqSException
}

func NewMqS() *MqS {
  println("NewMqS...")
  return (*MqS)(C.MqContextCreate(0,nil))
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC((*_Ctype_struct_MqS)(this), p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(m))
}

func (this *MqS) Exit(prefix string) {
  p := C.CString(prefix)
  C.MqExitP(p, (*_Ctype_struct_MqS)(this))
  C.free(unsafe.Pointer(p))
}

