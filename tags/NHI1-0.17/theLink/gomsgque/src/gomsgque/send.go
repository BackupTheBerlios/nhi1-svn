/**
 *  \file       theLink/gomsgque/src/gomsgque/send.go
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
)

func (this *MqS) SendSTART() {
  this.iErrorMqToGoWithCheck(C.MqSendSTART((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendEND_AND_WAIT(token string, timeout TIMEOUT) {
  t := C.CString(token)
  r := C.MqSendEND_AND_WAIT((*_Ctype_struct_MqS)(this), t, C.MQ_TIME_T(timeout))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) SendEND_AND_WAIT2(token string) {
  t := C.CString(token)
  r := C.MqSendEND_AND_WAIT((*_Ctype_struct_MqS)(this), t, C.MQ_TIMEOUT_DEFAULT)
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) SendEND_AND_CALLBACK(token string, cb Service) {
  t := C.CString(token)
  r := C.gomsgque_SendEND_AND_CALLBACK((*_Ctype_struct_MqS)(this), t, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
  incrServiceRef(&cb)
}

func (this *MqS) SendEND_AND_CALLBACK2(token string, cb Service2) {
  t := C.CString(token)
  r := C.gomsgque_SendEND_AND_CALLBACK2((*_Ctype_struct_MqS)(this), t, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
  incrService2Ref(&cb)
}

func (this *MqS) SendEND(token string) {
  t := C.CString(token)
  this.iErrorMqToGoWithCheck(C.MqSendEND((*_Ctype_struct_MqS)(this), t))
  C.free(unsafe.Pointer(t))
}

func (this *MqS) SendRETURN() {
  this.iErrorMqToGoWithCheck(C.MqSendRETURN((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendERROR() {
  this.iErrorMqToGoWithCheck(C.MqSendERROR((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendO(val bool) {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  this.iErrorMqToGoWithCheck(C.MqSendO((*_Ctype_struct_MqS)(this), v))
}

func (this *MqS) SendY(val int8) {
  this.iErrorMqToGoWithCheck(C.MqSendY((*_Ctype_struct_MqS)(this), C.MQ_BYT(val)))
}

func (this *MqS) SendS(val int16) {
  this.iErrorMqToGoWithCheck(C.MqSendS((*_Ctype_struct_MqS)(this), C.MQ_SRT(val)))
}

func (this *MqS) SendI(val int32) {
  this.iErrorMqToGoWithCheck(C.MqSendI((*_Ctype_struct_MqS)(this), C.MQ_INT(val)))
}

func (this *MqS) SendW(val int64) {
  this.iErrorMqToGoWithCheck(C.MqSendW((*_Ctype_struct_MqS)(this), C.MQ_WID(val)))
}

func (this *MqS) SendF(val float32) {
  this.iErrorMqToGoWithCheck(C.MqSendF((*_Ctype_struct_MqS)(this), C.MQ_FLT(val)))
}

func (this *MqS) SendD(val float64) {
  this.iErrorMqToGoWithCheck(C.MqSendD((*_Ctype_struct_MqS)(this), C.MQ_DBL(val)))
}

func (this *MqS) SendU(val *MqBufferS) {
  this.iErrorMqToGoWithCheck(C.MqSendU((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqBufferS)(val)))
}

func (this *MqS) SendB(val *MqBinary) {
  this.iErrorMqToGoWithCheck(C.MqSendB((*_Ctype_struct_MqS)(this), C.MQ_CBI(val.D), C.MQ_SIZE(val.L)))
}

func (this *MqS) SendN(val *MqBinary) {
  this.iErrorMqToGoWithCheck(C.MqSendN((*_Ctype_struct_MqS)(this), C.MQ_CBI(val.D), C.MQ_SIZE(val.L)))
}

func (this *MqS) SendC(val string) {
  s := C.CString(val)
  ret := C.MqSendC((*_Ctype_struct_MqS)(this), s)
  C.free(unsafe.Pointer(s))
  this.iErrorMqToGoWithCheck(ret)
}

func (this *MqS) SendL_START() {
  this.iErrorMqToGoWithCheck(C.MqSendL_START((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendL_END() {
  this.iErrorMqToGoWithCheck(C.MqSendL_END((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendT_START() {
  this.iErrorMqToGoWithCheck(C.MqSendT_START((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendT_END(tok string) {
  t := C.CString(tok)
  r := C.MqSendT_END((*_Ctype_struct_MqS)(this), t)
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
}

