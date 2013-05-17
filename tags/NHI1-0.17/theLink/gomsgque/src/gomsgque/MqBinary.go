/**
 *  \file       theLink/gomsgque/src/gomsgque/MqBinary.go
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

type MqBinary struct {
  D unsafe.Pointer
  L int
}

func (this *MqBinary) Set(val []byte) *MqBinary {
  this.D = unsafe.Pointer(&val[0])
  this.L = len(val)
  return this
}

func (this *MqBinary) Get() []byte {
  byt := make([]byte, this.L)
  C.memcpy(unsafe.Pointer(&byt[0]),this.D,C.size_t(this.L))
  return byt
}

