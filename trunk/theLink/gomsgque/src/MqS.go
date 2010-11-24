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

// for toplevel "*MqS"
func NewMqS() *MqS {
  ret := C.MqContextCreate(0,nil)
  // save the pointer, use "SetSelf" to create link to toplevel object
  lock[(*MqS)(ret)] = nil
  // cleanup "lock" after "object" is deleted
  C.gomsgque_ConfigSetFactory(ret, nil)
  // set default action for startup (check for "left over arguments")
  C.gomsgque_ConfigSetSetup(ret)
  return (*MqS)(ret)
}

// for embedding "MqS"
func (this *MqS) Init() {
  C.MqContextInit((*_Ctype_struct_MqS)(this), 0,nil)
}

// set link between *MqS and toplevel object
func (ctx *MqS) SetSelf(ifc interface{}) {
  lock[ctx] = ifc
}

// get toplevel object from *MqS
func (ctx *MqS) GetSelf() interface{} {
  return lock[ctx]
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

