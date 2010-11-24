/**
 *  \file       theLink/gomsgque/src/service.go
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

type Service interface {
  Call(*MqS)
}

//export gomsgque_cService
func (this *MqS) cService(cb Service) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  cb.Call(this)
}

func (this *MqS) ServiceCreate(token string, cb Service) {
  t := C.CString(token)
  C.gomsgque_ServiceCreate((*_Ctype_struct_MqS)(this), t, unsafe.Pointer(&cb))
  C.free(unsafe.Pointer(t))
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

func (this *MqS) ProcessEvent(timeout TIMEOUT, wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIME_T(timeout), uint32(wait)))
}

func (this *MqS) ProcessEvent2(wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIMEOUT_DEFAULT, uint32(wait)))
}

func (this *MqS) ServiceGetToken() string {
  return C.GoString(C.MqServiceGetToken((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ServiceGetFilter(i int32) *MqS {
  ret := C.MqServiceGetFilter2((*_Ctype_struct_MqS)(this), C.MQ_SIZE(i))
  if ret == nil {
    this.ErrorRaise()
  }
  return (*MqS)(ret)
}

func (this *MqS) ServiceGetFilter2() *MqS {
  ret := C.MqServiceGetFilter2((*_Ctype_struct_MqS)(this), 0)
  if ret == nil {
    this.ErrorRaise()
  }
  return (*MqS)(ret)
}

func (this *MqS) ServiceProxy(token string, i int32) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqServiceProxy((*_Ctype_struct_MqS)(this), t, C.MQ_SIZE(i)))
  C.free(unsafe.Pointer(t))
}

func (this *MqS) ServiceProxy2(token string) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqServiceProxy((*_Ctype_struct_MqS)(this), t, 0))
  C.free(unsafe.Pointer(t))
}
