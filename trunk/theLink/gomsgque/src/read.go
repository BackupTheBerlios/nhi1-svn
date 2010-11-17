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

func (this *MqS) ReadO() (MqSException, bool) {
  tmp := C.MQ_BOL(0)
  return MqSException(C.MqReadO(this.ctx, &tmp)), tmp != C.MQ_BOL(0)
}

func (this *MqS) ReadY() (MqSException, int8) {
  tmp := C.MQ_BYT(0)
  return MqSException(C.MqReadY(this.ctx, &tmp)), int8(tmp)
}

func (this *MqS) ReadS() (MqSException, int16) {
  tmp := C.MQ_SRT(0)
  return MqSException(C.MqReadS(this.ctx, &tmp)), int16(tmp)
}

func (this *MqS) ReadI() (MqSException, int32) {
  tmp := C.MQ_INT(0)
  return MqSException(C.MqReadI(this.ctx, &tmp)), int32(tmp)
}

func (this *MqS) ReadW() (MqSException, int64) {
  tmp := C.MQ_WID(0)
  return MqSException(C.MqReadW(this.ctx, &tmp)), int64(tmp)
}

func (this *MqS) ReadF() (MqSException, float32) {
  tmp := C.MQ_FLT(0.0)
  return MqSException(C.MqReadF(this.ctx, &tmp)), float32(tmp)
}

func (this *MqS) ReadD() (MqSException, float64) {
  tmp := C.MQ_DBL(0.0)
  return MqSException(C.MqReadD(this.ctx, &tmp)), float64(tmp)
}

func (this *MqS) ReadN() (MqSException, MqBinary) {
  var tmp C.MQ_CBI
  len := C.MQ_SIZE(0)
  return MqSException(C.MqReadN(this.ctx, &tmp, &len)), MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadB() (MqSException, MqBinary) {
  var tmp C.MQ_BIN
  len := C.MQ_SIZE(0)
  return MqSException(C.MqReadB(this.ctx, &tmp, &len)), MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqS) ReadU() (MqSException, C.MQ_BUF) {
  var tmp C.MQ_BUF
  return MqSException(C.MqReadU(this.ctx, &tmp)), tmp
}

func (this *MqS) ReadL_START(buf C.MQ_BUF) MqSException {
  return MqSException(C.MqReadL_START(this.ctx, buf))
}

func (this *MqS) ReadL_END() MqSException {
  return MqSException(C.MqReadL_END(this.ctx))
}

func (this *MqS) ReadT_START(buf C.MQ_BUF) MqSException {
  return MqSException(C.MqReadT_START(this.ctx, buf))
}

func (this *MqS) ReadT_END() MqSException {
  return MqSException(C.MqReadT_END(this.ctx))
}

func (this *MqS) ReadProxy(ctx *MqS) MqSException {
  return MqSException(C.MqReadProxy(this.ctx, ctx.ctx))
}

func (this *MqS) ReadGetNumItems(ctx *MqS) uint32 {
  return uint32(C.MqReadGetNumItems(this.ctx))
}

