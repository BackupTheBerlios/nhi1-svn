/**
 *  \file       theLink/gomsgque/src/gomsgque/MqSException.go
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
  "fmt"
  "unsafe"
)

type MqSException MqS

func (this *MqSException) String() string {
  return C.GoString(C.MqErrorGetText((*_Ctype_struct_MqS)(this)))
}

func (this *MqSException) GetText() string {
  return C.GoString(C.MqErrorGetText((*_Ctype_struct_MqS)(this)))
}

func (this *MqSException) GetNum() int32 {
  return int32(C.MqErrorGetNum((*_Ctype_struct_MqS)(this)))
}

func (this *MqS)  iErrorMqToGoWithCheck(ex uint32) {
  if (ex == C.MQ_ERROR) {
    panic((*MqSException)(this))
  }
}

func (this *MqS) ErrorSet(ex interface{}) {
  if ctx,ok := ex.(*MqSException); ok {
    C.MqErrorCopy((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqS)(ctx))
  } else if err,ok := ex.(error); ok {
    m := C.CString(err.Error())
    C.MqErrorC((*_Ctype_struct_MqS)(this), C.sERROR, -1, m)
    C.free(unsafe.Pointer(m))
  } else if err,ok := ex.(fmt.Stringer); ok {
    m := C.CString(err.String())
    C.MqErrorC((*_Ctype_struct_MqS)(this), C.sERROR, -1, m)
    C.free(unsafe.Pointer(m))
  } else {
    m := C.CString(fmt.Sprintf("%s", ex))
    C.MqErrorC((*_Ctype_struct_MqS)(this), C.sERROR, -1, m)
    C.free(unsafe.Pointer(m))
  }
/*
  // add stack-trace
  for i:=4; i<5; i++ {
    _,file,line,ok := runtime.Caller(i)
    if !ok { break }
    s := C.CString(fmt.Sprintf("called from %s(%d)", file, line))
      C.MqErrorSAppendC((*_Ctype_struct_MqS)(this), s)
    C.free(unsafe.Pointer(s))
  }
*/
}

func (this *MqS) ErrorC(prefix string, level int32, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqErrorC((*_Ctype_struct_MqS)(this), p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(m))
}

func (this *MqS) ErrorSetCONTINUE() {
  C.MqErrorSetCONTINUE((*_Ctype_struct_MqS)(this))
}

func (this *MqS) ErrorSetEXIT() {
  r := C.MqErrorSetEXITP((*_Ctype_struct_MqS)(this),C.sGO)
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) ErrorIsEXIT() bool {
  return C.MqErrorIsEXIT((*_Ctype_struct_MqS)(this)) == C.MQ_YES
}

func (this *MqS) ErrorReset() {
  C.MqErrorReset((*_Ctype_struct_MqS)(this))
}

func (this *MqS) ErrorRaise() {
  if (C.MqErrorGetCode((*_Ctype_struct_MqS)(this)) == C.MQ_ERROR) {
    panic((*MqSException)(this))
  }
}

func (this *MqS) ErrorPrint() {
  C.MqErrorPrint((*_Ctype_struct_MqS)(this))
}

func (this *MqS) ErrorGetText() string {
  return C.GoString(C.MqErrorGetText((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ErrorGetNum() int32 {
  return int32(C.MqErrorGetNum((*_Ctype_struct_MqS)(this)))
}

