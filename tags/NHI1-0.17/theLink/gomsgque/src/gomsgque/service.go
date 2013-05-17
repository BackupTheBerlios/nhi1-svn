/**
 *  \file       theLink/gomsgque/src/gomsgque/service.go
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

// #include "gomsgque.h"
import "C"

import (
  //"fmt"
  //"strings"
  "unsafe"
  //"runtime"
)

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "Service" interfaces
var lockService  = make(map[*Service]int)

func incrServiceRef(ifc *Service) {
  if count,ok := lockService[ifc]; ok {
    lockService[ifc] = count+1
  } else {
    lockService[ifc] = 1
  }
}

//export decrServiceRef
func decrServiceRef(ifc *Service) {
  if count,ok := lockService[ifc]; ok {
    if count > 1 {
      lockService[ifc]--
    } else {
      lockService[ifc] = 0
    }
  }
}

type Service interface {
  Call()
}

//export cService
func cService(this *MqS, cb *Service) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).Call()
}

func (this *MqS) ServiceCreate(token string, cb Service) {
  t := C.CString(token)
  r := C.gomsgque_ServiceCreate((*_Ctype_struct_MqS)(this), t, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
  incrServiceRef(&cb)
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "Service2" interfaces
var lockService2  = make(map[*Service2]int)

func incrService2Ref(ifc *Service2) {
  if count,ok := lockService2[ifc]; ok {
    lockService2[ifc] = count+1
  } else {
    lockService2[ifc] = 1
  }
}

//export decrService2Ref
func decrService2Ref(ifc *Service2) {
  if count,ok := lockService2[ifc]; ok {
    if count > 1 {
      lockService2[ifc]--
    } else {
      lockService2[ifc] = 0
    }
  }
}

type Service2 interface {
  Call(*MqS)
}

//export cService2
func cService2(this *MqS, cb *Service2) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).Call(this)
}

func (this *MqS) ServiceCreate2(token string, cb Service2) {
  t := C.CString(token)
  r := C.gomsgque_ServiceCreate2((*_Ctype_struct_MqS)(this), t, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
  incrService2Ref(&cb)
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

func (this *MqS) ProcessEvent(wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIMEOUT_DEFAULT, uint32(wait)))
}

func (this *MqS) ProcessEvent2(timeout TIMEOUT, wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIME_T(timeout), uint32(wait)))
}

func (this *MqS) ServiceGetToken() string {
  return C.GoString(C.MqServiceGetToken((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ServiceGetFilter() *MqS {
  ret := C.MqServiceGetFilter2((*_Ctype_struct_MqS)(this), 0)
  if ret == nil {
    this.ErrorRaise()
  }
  return (*MqS)(ret)
}

func (this *MqS) ServiceGetFilter2(i int32) *MqS {
  ret := C.MqServiceGetFilter2((*_Ctype_struct_MqS)(this), C.MQ_SIZE(i))
  if ret == nil {
    this.ErrorRaise()
  }
  return (*MqS)(ret)
}

func (this *MqS) ServiceProxy(token string) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqServiceProxy((*_Ctype_struct_MqS)(this), t, 0))
  C.free(unsafe.Pointer(t))
}

func (this *MqS) ServiceProxy2(token string, i int32) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqServiceProxy((*_Ctype_struct_MqS)(this), t, C.MQ_SIZE(i)))
  C.free(unsafe.Pointer(t))
}

func (this *MqS) ServiceStorage(token string) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqServiceStorage((*_Ctype_struct_MqS)(this), t))
  C.free(unsafe.Pointer(t))
}
