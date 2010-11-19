/**
 *  \file       theLink/gomsgque/src/MqBufferS.go
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
  "unsafe"
)

type MqBufferS _Ctype_struct_MqBufferS

func (this *MqBufferS)  iErrorBufToGoWithCheck(ex uint32) {
  //if (ex == C.MQ_ERROR) {
  if (ex == 2) {
    panic(C.MqBufferGetContext((*_Ctype_struct_MqBufferS)(this)))
  }
}

func (this *MqBufferS)  GetO() bool {
  var tmp C.MQ_BOL
  this.iErrorBufToGoWithCheck(C.MqBufferGetO((*_Ctype_struct_MqBufferS)(this), &tmp))
  return tmp == C.MQ_YES
}

func (this *MqBufferS)  GetY() int8 {
  var tmp C.MQ_BYT
  this.iErrorBufToGoWithCheck(C.MqBufferGetY((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int8(tmp)
}

func (this *MqBufferS)  GetS() int16 {
  var tmp C.MQ_SRT
  this.iErrorBufToGoWithCheck(C.MqBufferGetS((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int16(tmp)
}

func (this *MqBufferS)  GetI() int32 {
  var tmp C.MQ_INT
  this.iErrorBufToGoWithCheck(C.MqBufferGetI((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int32(tmp)
}

func (this *MqBufferS)  GetW() int64 {
  var tmp C.MQ_WID
  this.iErrorBufToGoWithCheck(C.MqBufferGetW((*_Ctype_struct_MqBufferS)(this), &tmp))
  return int64(tmp)
}

func (this *MqBufferS)  GetF() float32 {
  var tmp C.MQ_FLT
  this.iErrorBufToGoWithCheck(C.MqBufferGetF((*_Ctype_struct_MqBufferS)(this), &tmp))
  return float32(tmp)
}

func (this *MqBufferS)  GetD() float64 {
  var tmp C.MQ_DBL
  this.iErrorBufToGoWithCheck(C.MqBufferGetD((*_Ctype_struct_MqBufferS)(this), &tmp))
  return float64(tmp)
}

func (this *MqBufferS)  GetC() string {
  var tmp C.MQ_CST
  this.iErrorBufToGoWithCheck(C.MqBufferGetC((*_Ctype_struct_MqBufferS)(this), &tmp))
  return C.GoString(tmp)
}

func (this *MqBufferS)  GetB() MqBinary {
  var tmp C.MQ_BIN
  var len C.MQ_SIZE
  this.iErrorBufToGoWithCheck(C.MqBufferGetB((*_Ctype_struct_MqBufferS)(this), &tmp, &len))
  return MqBinary{unsafe.Pointer(tmp), int(len)}
}

type MqTypeE uint32

const (
  MQ_BYTT MqTypeE = C.MQ_BYTT
  MQ_BOLT MqTypeE = C.MQ_BOLT
  MQ_SRTT MqTypeE = C.MQ_SRTT
  MQ_INTT MqTypeE = C.MQ_INTT
  MQ_FLTT MqTypeE = C.MQ_FLTT
  MQ_WIDT MqTypeE = C.MQ_WIDT
  MQ_DBLT MqTypeE = C.MQ_DBLT
  MQ_BINT MqTypeE = C.MQ_BINT
  MQ_STRT MqTypeE = C.MQ_STRT
  MQ_LSTT MqTypeE = C.MQ_LSTT
  MQ_TRAT MqTypeE = C.MQ_TRAT
)

func (this *MqBufferS)  GetType() MqTypeE {
  return MqTypeE(C.MqBufferGetType2((*_Ctype_struct_MqBufferS)(this)))
}
