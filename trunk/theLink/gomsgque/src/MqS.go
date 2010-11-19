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
  MqS_TIMEOUT_DEFAULT TIMEOUT	= C.MQ_TIMEOUT_DEFAULT
  MqS_TIMEOUT_USER    TIMEOUT	= C.MQ_TIMEOUT_USER
  MqS_TIMEOUT_MAX     TIMEOUT	= C.MQ_TIMEOUT_MAX

  MqS_WAIT_NO	      WAIT	= C.MQ_WAIT_NO
  MqS_WAIT_ONCE	      WAIT	= C.MQ_WAIT_ONCE
  MqS_WAIT_FOREVER    WAIT	= C.MQ_WAIT_FOREVER
)

type MqS _Ctype_struct_MqS

type MqSCallback func()

type IServerSetup interface {
  ServerSetup()
}

type IServerCleanup interface {
  ServerCleanup()
}

func NewMqS() *MqS {
  this := C.MqContextCreate(0,nil)
  C.MqConfigSetSelf(this, unsafe.Pointer(this))
println("NewMqS...", this)
  return (*MqS)(this)
}

func (this *MqS) String() string {
  return C.GoString(C.MqErrorGetText((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC((*_Ctype_struct_MqS)(this), p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(m))
  C.free(unsafe.Pointer(p))
}

func (this *MqS) Exit() {
  C.MqExitP(C.sGO, (*_Ctype_struct_MqS)(this))
}

