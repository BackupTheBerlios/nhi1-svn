/**
 *  \file       theLink/gomsgque/src/MqSException.go
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
  "os"
)

func (this *MqS)  iErrorMqToGoWithCheck(ex uint32) {
  if (ex == C.MQ_ERROR) {
println("iErrorMqToGoWithCheck...")
    panic(this)
  }
}

func (this *MqS) ErrorSet(ex interface{}) {
println("ErrorSet...")
  if ctx,ok := ex.(*MqS); ok {
    C.MqErrorCopy(this.mqctx, ctx.mqctx)
  } else if err,ok := ex.(os.Error); ok {
    m := C.CString(err.String())
    C.MqErrorC(this.mqctx, C.sGO, -1, m)
    C.free(unsafe.Pointer(m))
  } else {
    C.MqErrorC(this.mqctx, C.sGO, -1, C.sUNKNOWN)
  }
}

func (this *MqS) ErrorC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqErrorC(this.mqctx, p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(m))
}

func (this *MqS) ErrorSetCONTINUE() {
  C.MqErrorSetCONTINUE(this.mqctx)
}

func (this *MqS) ErrorSetEXIT() {
  r := C.MqErrorSetEXITP(this.mqctx,C.sGO)
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) ErrorIsEXIT() bool {
  return C.MqErrorIsEXIT(this.mqctx) == C.MQ_YES
}

func (this *MqS) ErrorReset() {
  C.MqErrorReset(this.mqctx)
}

func (this *MqS) ErrorPrint() {
  C.MqErrorPrint(this.mqctx)
}

