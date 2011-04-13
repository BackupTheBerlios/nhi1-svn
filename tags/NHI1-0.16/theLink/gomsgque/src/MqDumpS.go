/**
 *  \file       theLink/gomsgque/src/MqDumpS.go
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
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
  //  "unsafe"
)

type MqDumpS _Ctype_struct_MqDumpS

func (this *MqDumpS) Size() int32 {
  return int32(C.MqDumpSize((*_Ctype_struct_MqDumpS)(this)))
}

func (this *MqDumpS) Delete() {
  C.MqSysFreeP(C.MQ_PTR(this))
}
