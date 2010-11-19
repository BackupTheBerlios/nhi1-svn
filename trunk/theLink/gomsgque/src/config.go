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
  "fmt"
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
func (this *MqS) cServerSetup(cb *IServerSetup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  this.LogC("cServerSetup",0,"1111111111111111111111\n")
  (*cb).ServerSetup()
}

func (this *MqS) ConfigSetServerSetup(cb IServerSetup) {
  println("1. ConfigSetServerSetup ... ", cb)
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

func (this *MqS) ConfigSetServerCleanup(cb IServerCleanup) {
  fmt.Printf("ConfigSetServerCleanup ... %T\n", cb)
  C.MqConfigSetServerCleanup((*_Ctype_struct_MqS)(this), nil, nil, nil, nil)
}

