/**
 *  \file       theLink/gomsgque/src/gomsgque/MqBufferS.go
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
  "unsafe"
)

type MqBufferS _Ctype_struct_MqBufferS

func (this *MqBufferS)  iErrorBufToGoWithCheck(ex uint32) {
  if (ex == _Cconst_MQ_ERROR) {
    panic((*MqSException)(C.MqBufferGetContext((*_Ctype_struct_MqBufferS)(this))))
  }
}

func (this *MqBufferS)  Dup() *MqBufferS {
  return (*MqBufferS)(C.MqBufferDup((*_Ctype_struct_MqBufferS)(this)))
}

func (this *MqBufferS)  Delete() {
  tmp := (*_Ctype_struct_MqBufferS)(this)
  C.MqBufferDelete(&tmp)
}

func (this *MqBufferS)  GetO() bool {
  var tmp C.MQ_BOL
  this.iErrorBufToGoWithCheck(C.MqBufferGetO((*_Ctype_struct_MqBufferS)(this), &tmp))
  return tmp == C.MQ_YES
}

func (this *MqBufferS) SetO(val bool) *MqBufferS {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  return (*MqBufferS) (C.MqBufferSetO((*_Ctype_struct_MqBufferS)(this), v))
}

func (this *MqBufferS)  GetY() int8 {
  var tmp C.MQ_BYT
  this.iErrorBufToGoWithCheck(C.MqBufferGetY((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int8(tmp)
}

func (this *MqBufferS) SetY(val int8) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetY((*_Ctype_struct_MqBufferS)(this), C.MQ_BYT(val)))
}

func (this *MqBufferS)  GetS() int16 {
  var tmp C.MQ_SRT
  this.iErrorBufToGoWithCheck(C.MqBufferGetS((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int16(tmp)
}

func (this *MqBufferS) SetS(val int16) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetS((*_Ctype_struct_MqBufferS)(this), C.MQ_SRT(val)))
}

func (this *MqBufferS)  GetI() int32 {
  var tmp C.MQ_INT
  this.iErrorBufToGoWithCheck(C.MqBufferGetI((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int32(tmp)
}

func (this *MqBufferS) SetI(val int32) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetI((*_Ctype_struct_MqBufferS)(this), C.MQ_INT(val)))
}

func (this *MqBufferS)  GetW() int64 {
  var tmp C.MQ_WID
  this.iErrorBufToGoWithCheck(C.MqBufferGetW((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int64(tmp)
}

func (this *MqBufferS) SetW(val int64) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetW((*_Ctype_struct_MqBufferS)(this), C.MQ_WID(val)))
}

func (this *MqBufferS)  GetF() float32 {
  var tmp C.MQ_FLT
  this.iErrorBufToGoWithCheck(C.MqBufferGetF((*_Ctype_struct_MqBufferS)(this), &tmp))
  return float32(tmp)
}

func (this *MqBufferS) SetF(val float32) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetF((*_Ctype_struct_MqBufferS)(this), C.MQ_FLT(val)))
}

func (this *MqBufferS)  GetD() float64 {
  var tmp C.MQ_DBL
  this.iErrorBufToGoWithCheck(C.MqBufferGetD((*_Ctype_struct_MqBufferS)(this), &tmp))
  return float64(tmp)
}

func (this *MqBufferS) SetD(val float64) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetD((*_Ctype_struct_MqBufferS)(this), C.MQ_DBL(val)))
}

func (this *MqBufferS)  GetC() string {
  var tmp C.MQ_CST
  this.iErrorBufToGoWithCheck(C.MqBufferGetC((*_Ctype_struct_MqBufferS)(this), &tmp))
  return C.GoString(tmp)
}

func (this *MqBufferS) SetC(val string) *MqBufferS {
  s := C.CString(val)
  ret := C.MqBufferSetC((*_Ctype_struct_MqBufferS)(this), C.MQ_CST(s))
  C.free(unsafe.Pointer(s))
  return (*MqBufferS) (ret)
}

func (this *MqBufferS)  GetB() *MqBinary {
  var tmp C.MQ_BIN
  var len C.MQ_SIZE
  this.iErrorBufToGoWithCheck(C.MqBufferGetB((*_Ctype_struct_MqBufferS)(this), &tmp, &len))
  return &MqBinary{unsafe.Pointer(tmp), int(len)}
}

func (this *MqBufferS) SetB(val *MqBinary) *MqBufferS {
  return (*MqBufferS) (C.MqBufferSetB((*_Ctype_struct_MqBufferS)(this), C.MQ_CBI(val.D), C.MQ_SIZE(val.L)))
}

func (this *MqBufferS)  GetType() string {
  return string(C.MqBufferGetType((*_Ctype_struct_MqBufferS)(this)))
}

