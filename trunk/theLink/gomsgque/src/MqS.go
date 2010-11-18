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
  "unsafe"
  //"reflect"
)

type TIMEOUT  C.MQ_TIME_T
type WAIT     uint32

const (
  TIMEOUT_DEFAULT TIMEOUT = C.MQ_TIMEOUT_DEFAULT
  TIMEOUT_USER	  TIMEOUT = C.MQ_TIMEOUT_USER
  TIMEOUT_MAX	  TIMEOUT = C.MQ_TIMEOUT_MAX

  WAIT_NO	  WAIT	= C.MQ_WAIT_NO
  WAIT_ONCE	  WAIT	= C.MQ_WAIT_ONCE
  WAIT_FOREVER	  WAIT	= C.MQ_WAIT_FOREVER
)

type MqS struct {
  mqctx *_Ctype_struct_MqS
}

type MqCallback func()

type IServerSetup interface {
  ServerSetup()
}

type IServerCleanup interface {
  ServerCleanup()
}

func NewMqS() *MqS {
  this := new(MqS)
  this.mqctx = C.MqContextCreate(0,nil)
  C.MqConfigSetSelf(this.mqctx, unsafe.Pointer(this))
println("NewMqS...", this, this.mqctx)
  return this
}

func (this *MqS) String() string {
  return C.GoString(C.MqErrorGetText(this.mqctx))
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC(this.mqctx, p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(m))
  C.free(unsafe.Pointer(p))
}

func (this *MqS) Exit() {
  C.MqExitP(C.sGO, this.mqctx)
}

