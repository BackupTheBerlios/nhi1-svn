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
  //"strings"
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
func (this *MqS) cServerSetup() MqSException {
  println("cServerSetup...")
  this.LogC("cServerSetup",0,"1111111111111111111111\n")
  //return MqSException(OK)
  return this.ErrorC("cServerSetup",-1,"this is an test error")
}

func (this *MqS) ConfigSetServerSetup(ifc IServerSetup) {
  fmt.Printf("ConfigSetServerSetup ... %T\n", ifc)
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this))
}

func (this *MqS) ConfigSetServerCleanup(ifc IServerCleanup) {
  fmt.Printf("ConfigSetServerCleanup ... %T\n", ifc)
  C.MqConfigSetServerCleanup((*_Ctype_struct_MqS)(this), nil, nil, nil, nil)
}



