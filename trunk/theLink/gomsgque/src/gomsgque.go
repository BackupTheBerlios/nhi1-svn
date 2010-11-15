/**
 *  \file       theLink/gomsgque/src/gomsgque.go
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
#include <stdlib.h>
#include <msgque.h>

*/
import "C"

import (
  "unsafe"
)

type MqS struct {
  ctx *_Ctype_struct_MqS
}

func NewMqS() *MqS {
  var ret = new(MqS)
  ret.ctx = C.MqContextCreate(0,nil)
  return ret
}

func (this *MqS) LinkCreate() {
  C.MqLinkCreate(this.ctx, nil)
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC(this.ctx, p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(m))
}

func (this *MqS) ConfigSetName(val string) {
  v := C.CString(val)
  C.MqConfigSetName(this.ctx, v)
  C.free(unsafe.Pointer(v))
}


