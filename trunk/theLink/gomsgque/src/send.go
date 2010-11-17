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

func (this *MqS) SendSTART() MqSException {
  return MqSException(C.MqSendSTART(this.ctx))
}

func (this *MqS) SendEND_AND_WAIT(token string, timeout int64) MqSException {
  t := C.CString(token)
  r := C.MqSendEND_AND_WAIT(this.ctx, t, C.MQ_TIME_T(timeout))
  C.free(unsafe.Pointer(t))
  return MqSException(r)
}

func (this *MqS) SendI(val int32) MqSException {
  return MqSException(C.MqSendI(this.ctx, C.MQ_INT(val)))
}


