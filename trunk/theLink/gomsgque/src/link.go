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

/*
#include <gomsgque.h>
*/
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

func (this *MqS) LinkGetTargetIdent() string {
  return C.GoString(C.MqLinkGetTargetIdent((*_Ctype_struct_MqS)(this)))
}






