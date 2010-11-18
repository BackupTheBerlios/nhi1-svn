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
  C.MqConfigSetName(this.mqctx, v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) ConfigSetIdent(val string) {
  v := C.CString(val)
  C.MqConfigSetIdent(this.mqctx, v)
  C.free(unsafe.Pointer(v))
}

//export cServerSetup
func (this *MqS) cServerSetup(c *MqCallback) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
println("cServerSetup... this=", this)
  this.LogC("cServerSetup",0,"1111111111111111111111\n")
  (*c)()
  this.LogC("cServerSetup",0,"2222222222222222222222\n")
}

func (this *MqS) ConfigSetServerSetup(cb MqCallback) {
  fmt.Printf("ConfigSetServerSetup ... %T\n", cb)
  C.gomsgque_ConfigSetServerSetup(this.mqctx, unsafe.Pointer(&cb))
}

func (this *MqS) ConfigSetServerCleanup(ifc IServerCleanup) {
  fmt.Printf("ConfigSetServerCleanup ... %T\n", ifc)
  C.MqConfigSetServerCleanup(this.mqctx, nil, nil, nil, nil)
}







