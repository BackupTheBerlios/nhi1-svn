/**
 *  \file       theLink/gomsgque/src/service.go
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

//export cService
func (this *MqS) cService(cb *Service) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).Call()
}

func (this *MqS) ServiceCreate(token string, cb Service) {
  t := C.CString(token)
  C.gomsgque_ServiceCreate((*_Ctype_struct_MqS)(this), t, unsafe.Pointer(&cb))
  C.free(unsafe.Pointer(t))
}

func (this *MqS) ProcessEvent(timeout TIMEOUT, wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIME_T(timeout), uint32(wait)))
}


