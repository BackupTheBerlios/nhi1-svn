/**
 *  \file       theLink/gomsgque/src/MqS.go
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
package MqS

/*
#include <stdlib.h>
#include <msgque.h>

typedef signed int* MQ_INTP;

*/
import "C"

import (
  // "fmt"
  "unsafe"
)

const (
  OK		  = 0
  CONTINUE	  = 1
  ERROR		  = 2

  TIMEOUT_DEFAULT = -1
  TIMEOUT_USER	  = -2
  TIMEOUT_MAX	  = -3
)

type MqS struct {
  ctx *_Ctype_struct_MqS
}

func NewMqS() *MqS {
  var ret = new(MqS)
  ret.ctx = C.MqContextCreate(0,nil)
  return ret
}

func (this *MqS) LinkCreate(argv ... string) uint32 {
  var largv *_Ctype_struct_MqBufferLS
  if (len(argv) != 0) {
    largv = C.MqBufferLCreate(C.MQ_SIZE(len(argv)));
    for idx:= range argv {
      a := C.CString(argv[idx])
      C.MqBufferLAppendC(largv, a);
      C.free(unsafe.Pointer(a))
    }
  }
  return C.MqLinkCreate(this.ctx, &largv)
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC(this.ctx, p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(m))
}

func (this *MqS) Exit(prefix string) {
  p := C.CString(prefix)
  C.MqExitP(p, this.ctx)
  C.free(unsafe.Pointer(p))
}

func (this *MqS) ConfigSetName(val string) {
  v := C.CString(val)
  C.MqConfigSetName(this.ctx, v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) SendSTART() uint32 {
  return C.MqSendSTART(this.ctx)
}

func (this *MqS) SendEND_AND_WAIT(token string, timeout int32) uint32 {
  t := C.CString(token)
  r := C.MqSendEND_AND_WAIT(this.ctx, t, C.MQ_TIME_T(timeout))
  C.free(unsafe.Pointer(t))
  return r
}

func (this *MqS) SendI(val int32) uint32 {
  return C.MqSendI(this.ctx, C.MQ_INT(val))
}

func (this *MqS) ReadI(val *int32) uint32 {
  tmp := C.MQ_INT(0)
  ret := uint32(OK)
  *val = 0
  ret = C.MqReadI(this.ctx, &tmp)
  if (ret == ERROR) {goto error}
  *val = int32(tmp)
error:
  return ret
}


