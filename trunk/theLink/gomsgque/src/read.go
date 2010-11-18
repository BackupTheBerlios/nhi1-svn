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
  tmp := C.MQ_BOL(0)
  this.iErrorMqToGoWithCheck(C.MqReadO(this.mqctx, &tmp))
  return tmp != C.MQ_BOL(0)
}

func (this *MqS) ReadY() int8 {
  tmp := C.MQ_BYT(0)
  this.iErrorMqToGoWithCheck(C.MqReadY(this.mqctx, &tmp))
  return int8(tmp)
}

func (this *MqS) ReadS() int16 {
  tmp := C.MQ_SRT(0)
  this.iErrorMqToGoWithCheck(C.MqReadS(this.mqctx, &tmp))
  return int16(tmp)
}

func (this *MqS) ReadI() int32 {
  tmp := C.MQ_INT(0)
  this.iErrorMqToGoWithCheck(C.MqReadI(this.mqctx, &tmp))
  return int32(tmp)
}

func (this *MqS) ReadW() int64 {
  tmp := C.MQ_WID(0)
  this.iErrorMqToGoWithCheck(C.MqReadW(this.mqctx, &tmp))
  return int64(tmp)
}

func (this *MqS) ReadF() float32 {
  tmp := C.MQ_FLT(0.0)
  this.iErrorMqToGoWithCheck(C.MqReadF(this.mqctx, &tmp))
  return float32(tmp)
}

func (this *MqS) ReadD() float64 {
  tmp := C.MQ_DBL(0.0)
  this.iErrorMqToGoWithCheck(C.MqReadD(this.mqctx, &tmp))
  return float64(tmp)
}

func (this *MqS) ReadN() MqBinary {
  var tmp C.MQ_CBI
  len := C.MQ_SIZE(0)
  this.iErrorMqToGoWithCheck(C.MqReadN(this.mqctx, &tmp, &len))
  return MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadB() MqBinary {
  var tmp C.MQ_BIN
  len := C.MQ_SIZE(0)
  this.iErrorMqToGoWithCheck(C.MqReadB(this.mqctx, &tmp, &len))
  return MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadU() C.MQ_BUF {
  var tmp C.MQ_BUF
  this.iErrorMqToGoWithCheck(C.MqReadU(this.mqctx, &tmp))
  return tmp
}

func (this *MqS) ReadL_START(buf C.MQ_BUF) {
  this.iErrorMqToGoWithCheck(C.MqReadL_START(this.mqctx, buf))
}

func (this *MqS) ReadL_END() {
  this.iErrorMqToGoWithCheck(C.MqReadL_END(this.mqctx))
}

func (this *MqS) ReadT_START(buf C.MQ_BUF) {
  this.iErrorMqToGoWithCheck(C.MqReadT_START(this.mqctx, buf))
}

func (this *MqS) ReadT_END() {
  this.iErrorMqToGoWithCheck(C.MqReadT_END(this.mqctx))
}

func (this *MqS) ReadProxy(ctx *MqS) {
  this.iErrorMqToGoWithCheck(C.MqReadProxy(this.mqctx, ctx.mqctx))
}

func (this *MqS) ReadGetNumItems(ctx *MqS) uint32 {
  return uint32(C.MqReadGetNumItems(this.mqctx))
}


