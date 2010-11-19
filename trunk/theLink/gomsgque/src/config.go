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

//export cServerSetup
func (this *MqS) cServerSetup(cb *ServerSetup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).ServerSetup()
}

func (this *MqS) ConfigSetServerSetup(cb ServerSetup) {
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

func (this *MqS) ConfigSetServerCleanup(cb ServerCleanup) {
  C.MqConfigSetServerCleanup((*_Ctype_struct_MqS)(this), nil, nil, nil, nil)
}

