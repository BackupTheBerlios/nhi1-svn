/**
 *  \file       theLink/gomsgque/src/gomsgque/slave.go
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

// #include "gomsgque.h"
import "C"

import (
  //"fmt"
  //"strings"
  "unsafe"
)

func (this *MqS) SlaveWorker(id int32, argv ... string) {
  var largv *_Ctype_struct_MqBufferLS
  if (len(argv) != 0) {
    largv = C.MqBufferLCreate(C.MQ_SIZE(len(argv)));
    for idx:= range argv {
	a := C.CString(argv[idx])
      C.MqBufferLAppendC(largv, a);
	C.free(unsafe.Pointer(a))
    }
  }
  this.iErrorMqToGoWithCheck(C.MqSlaveWorker((*_Ctype_struct_MqS)(this), C.MQ_SIZE(id), &largv))
}

func (this *MqS) SlaveCreate(id int32, slv *MqS) {
  this.iErrorMqToGoWithCheck(C.MqSlaveCreate((*_Ctype_struct_MqS)(this),
      C.MQ_SIZE(id), (*_Ctype_struct_MqS)(slv)))
}

func (this *MqS) SlaveDelete(id int32) {
  this.iErrorMqToGoWithCheck(C.MqSlaveDelete((*_Ctype_struct_MqS)(this), C.MQ_SIZE(id)))
}

func (this *MqS) SlaveGet(id int32) *MqS {
  return (*MqS)(C.MqSlaveGet((*_Ctype_struct_MqS)(this), C.MQ_SIZE(id)))
}

func (this *MqS) SlaveGetMaster() *MqS {
  return (*MqS)(C.MqSlaveGetMaster((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) SlaveIs() bool {
  return (bool)(C.MqSlaveIs((*_Ctype_struct_MqS)(this)))
}

