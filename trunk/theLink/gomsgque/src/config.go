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
#include <stdlib.h>
#include <msgque.h>
*/
import "C"

import (
  //"fmt"
  //"strings"
  "unsafe"
)

func (this *MqS) ConfigSetName(val string) {
  v := C.CString(val)
  C.MqConfigSetName(this.ctx, v)
  C.free(unsafe.Pointer(v))
}

