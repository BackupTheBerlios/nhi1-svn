/**
 *  \file       theLink/gomsgque/src/read.go
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

func (this *MqS) ReadO() bool {
  var tmp C.MQ_BOL
  this.iErrorMqToGoWithCheck(C.MqReadO((*_Ctype_struct_MqS)(this), &tmp))
  return tmp == C.MQ_YES
}

func (this *MqS) ReadY() int8 {
  var tmp C.MQ_BYT
  this.iErrorMqToGoWithCheck(C.MqReadY((*_Ctype_struct_MqS)(this), &tmp))
  return int8(tmp)
}

func (this *MqS) ReadS() int16 {
  var tmp C.MQ_SRT
  this.iErrorMqToGoWithCheck(C.MqReadS((*_Ctype_struct_MqS)(this), &tmp))
  return int16(tmp)
}

func (this *MqS) ReadI() int32 {
  var tmp C.MQ_INT
  this.iErrorMqToGoWithCheck(C.MqReadI((*_Ctype_struct_MqS)(this), &tmp))
  return int32(tmp)
}

func (this *MqS) ReadW() int64 {
  var tmp C.MQ_WID
  this.iErrorMqToGoWithCheck(C.MqReadW((*_Ctype_struct_MqS)(this), &tmp))
  return int64(tmp)
}

func (this *MqS) ReadF() float32 {
  var tmp C.MQ_FLT
  this.iErrorMqToGoWithCheck(C.MqReadF((*_Ctype_struct_MqS)(this), &tmp))
  return float32(tmp)
}

func (this *MqS) ReadD() float64 {
  var tmp C.MQ_DBL
  this.iErrorMqToGoWithCheck(C.MqReadD((*_Ctype_struct_MqS)(this), &tmp))
  return float64(tmp)
}

func (this *MqS) ReadC() string {
  var tmp C.MQ_CST
  this.iErrorMqToGoWithCheck(C.MqReadC((*_Ctype_struct_MqS)(this), &tmp))
  return C.GoString(tmp)
}

func (this *MqS) ReadN() MqBinary {
  var tmp C.MQ_CBI
  var len C.MQ_SIZE
  this.iErrorMqToGoWithCheck(C.MqReadN((*_Ctype_struct_MqS)(this), &tmp, &len))
  return MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadB() MqBinary {
  var tmp C.MQ_BIN
  var len C.MQ_SIZE
  this.iErrorMqToGoWithCheck(C.MqReadB((*_Ctype_struct_MqS)(this), &tmp, &len))
  return MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadU() *MqBufferS {
  var tmp *_Ctype_struct_MqBufferS
  this.iErrorMqToGoWithCheck(C.MqReadU((*_Ctype_struct_MqS)(this), &tmp))
  return (*MqBufferS)(tmp)
}

func (this *MqS) ReadL_START(buf *MqBufferS) {
  this.iErrorMqToGoWithCheck(C.MqReadL_START((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqBufferS)(buf)))
}

func (this *MqS) ReadL_END() {
  this.iErrorMqToGoWithCheck(C.MqReadL_END((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ReadT_START(buf *MqBufferS) {
  this.iErrorMqToGoWithCheck(C.MqReadT_START((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqBufferS)(buf)))
}

func (this *MqS) ReadT_END() {
  this.iErrorMqToGoWithCheck(C.MqReadT_END((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ReadProxy(ctx *MqS) {
  this.iErrorMqToGoWithCheck(C.MqReadProxy((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqS)(ctx)))
}

func (this *MqS) ReadGetNumItems() uint32 {
  return uint32(C.MqReadGetNumItems((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ReadItemExists() bool {
  return C.MqReadItemExists((*_Ctype_struct_MqS)(this)) == C.MQ_YES
}

func (this *MqS) ReadUndo() {
  C.MqReadUndo((*_Ctype_struct_MqS)(this))
}

