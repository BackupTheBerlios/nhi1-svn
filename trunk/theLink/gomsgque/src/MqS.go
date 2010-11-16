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

package gomsgque

/*
#include <stdlib.h>
#include <msgque.h>
*/
import "C"

import (
  //"fmt"
  //"strings"
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
  return &MqS{C.MqContextCreate(0,nil)}
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

