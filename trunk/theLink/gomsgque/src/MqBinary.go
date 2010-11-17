/**
 *  \file       theLink/gomsgque/src/MqBinary.go
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

type MqBinary struct {
  D unsafe.Pointer
  L int
}

func (this MqBinary) array() []byte {
  byt := make([]byte, this.L)
  C.memcpy(unsafe.Pointer(&byt[0]),this.D,C.size_t(this.L))
  return byt
}

func (this MqBinary) string() string {
  return C.GoStringN((*C.char)(this.D), C.int(this.L))
}

