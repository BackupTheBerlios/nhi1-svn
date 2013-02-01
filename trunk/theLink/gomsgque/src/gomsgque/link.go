/**
 *  \file       theLink/gomsgque/src/link.go
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

func (this *MqS) LinkCreate(argv ... string) {
  var largv *_Ctype_struct_MqBufferLS
  //fmt.Println("argv = " + strings.Join(argv,","))
  if (len(argv) != 0) {
    largv = C.MqBufferLCreate(C.MQ_SIZE(len(argv)));
    for idx:= range argv {
	a := C.CString(argv[idx])
      C.MqBufferLAppendC(largv, a);
	C.free(unsafe.Pointer(a))
    }
  }
  this.iErrorMqToGoWithCheck(C.MqLinkCreate((*_Ctype_struct_MqS)(this), &largv))
}

func (this *MqS) LinkCreateChild(parent *MqS, argv ... string) {
  var largv *_Ctype_struct_MqBufferLS
  //fmt.Println("LinkCreateChild -> argv = " + strings.Join(argv,","))
  if (len(argv) != 0) {
    largv = C.MqBufferLCreate(C.MQ_SIZE(len(argv)));
    for idx:= range argv {
	a := C.CString(argv[idx])
      C.MqBufferLAppendC(largv, a);
	C.free(unsafe.Pointer(a))
    }
  }
  this.iErrorMqToGoWithCheck(C.MqLinkCreateChild((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqS)(parent), &largv))
}

func (this *MqS) LinkDelete() {
  C.MqLinkDelete((*_Ctype_struct_MqS)(this))
}

func (this *MqS) LinkGetTargetIdent() string {
  return C.GoString(C.MqLinkGetTargetIdent((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) LinkGetCtxId() int32 {
  return int32(C.MqLinkGetCtxId((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) LinkIsParent() bool {
  return bool(C.MqLinkIsParent((*_Ctype_struct_MqS)(this)) != 0)
}

func (this *MqS) LinkGetParent() *MqS {
  return (*MqS)(C.MqLinkGetParent((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) LinkIsConnected() bool {
  return C.MqLinkIsConnected((*_Ctype_struct_MqS)(this)) != 0
}

func (this *MqS) LinkConnect() {
  this.iErrorMqToGoWithCheck(C.MqLinkConnect((*_Ctype_struct_MqS)(this)))
}

