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
  //"unsafe"
)

func (this *MqS) ProcessEvent(timeout TIMEOUT, wait WAIT) {
  this.iErrorMqToGoWithCheck(C.MqProcessEvent((*_Ctype_struct_MqS)(this), C.MQ_TIME_T(timeout), uint32(wait)))
}






