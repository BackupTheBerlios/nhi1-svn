/**
 *  \file       theLink/gomsgque/src/send.go
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

func (this *MqS) SendSTART() {
  this.iErrorMqToGoWithCheck(C.MqSendSTART((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendEND_AND_WAIT(token string, timeout TIMEOUT) {
  t := C.CString(token)
  r := C.MqSendEND_AND_WAIT((*_Ctype_struct_MqS)(this), t, C.MQ_TIME_T(timeout))
  C.free(unsafe.Pointer(t))
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) SendRETURN() {
  this.iErrorMqToGoWithCheck(C.MqSendRETURN((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SendI(val int32) {
  this.iErrorMqToGoWithCheck(C.MqSendI((*_Ctype_struct_MqS)(this), C.MQ_INT(val)))
}

