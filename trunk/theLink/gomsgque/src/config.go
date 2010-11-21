/**
 *  \file       theLink/gomsgque/src/config.go
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
)

func (this *MqS) ConfigSetName(val string) {
  v := C.CString(val)
  C.MqConfigSetName((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) ConfigSetIdent(val string) {
  v := C.CString(val)
  C.MqConfigSetIdent((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
}

//export gomsgque_cServerSetup
func cServerSetup(this *MqS, cb *ServerSetup) uint32 {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).ServerSetup(this)
  return this.error.code
}

func (this *MqS) ConfigSetServerSetup(cb ServerSetup) {
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

//export gomsgque_cServerCleanup
func cServerCleanup(this *MqS, cb *ServerCleanup) uint32 {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).ServerCleanup(this)
  return this.error.code
}

func (this *MqS) ConfigSetServerCleanup(cb ServerCleanup) {
  C.gomsgque_ConfigSetServerCleanup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

