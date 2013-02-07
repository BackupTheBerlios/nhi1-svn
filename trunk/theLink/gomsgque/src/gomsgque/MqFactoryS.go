/**
 *  \file       theLink/gomsgque/src/gomsgque/MqFactoryS.go
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
  //"os"
  "unsafe"
)

/*****************************************************************************/
/*                                                                           */
/*                                 private                                   */
/*                                                                           */
/*****************************************************************************/

type FactoryF func(*MqS) (*MqS)

// global lock for "Factory" interfaces
var lockFactory  = make(map[*FactoryF]int)

//export incrFactoryRef
func incrFactoryRef(cb *FactoryF) {
  if count,ok := lockFactory[cb]; ok {
    lockFactory[cb] = count+1
  } else {
    lockFactory[cb] = 1
  }
}

//export decrFactoryRef
func decrFactoryRef(cb *FactoryF) {
  if count,ok := lockFactory[cb]; ok {
    if count > 1 {
      lockFactory[cb]--
    } else {
      lockFactory[cb] = 0
    }
  }
}

//export cFactoryCall
func cFactoryCall(tmpl *MqS, cb *FactoryF) (ret *MqS) {
  defer func() {
    if (tmpl != nil) {
      if x := recover(); x != nil {
	tmpl.ErrorSet(x)
	ret = nil
      }
    }
  }()
  if (cb == nil) {
    ret = NewMqS(tmpl,nil)
  } else {
    ret = (*cb)(tmpl)
  }
  return
}

//export cFactoryDelete
func cFactoryDelete(this *MqS, chnp *chan bool) {
  ctxlock[this] = 0
  if chnp != nil {
    (*chnp) <- true
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                 context                                   */
/*                                                                           */
/*****************************************************************************/

func (this *MqS) FactoryCtxGet() *MqFactoryS {
  return (*MqFactoryS)(C.MqFactoryCtxGet((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) FactoryCtxSet(val *MqFactoryS) {
  this.iErrorMqToGoWithCheck(C.MqFactoryCtxSet((*_Ctype_struct_MqS)(this), (*_Ctype_struct_MqFactoryS)(val)))
}

func (this *MqS) FactoryCtxIdentGet() string {
  return C.GoString(C.MqFactoryCtxIdentGet((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) FactoryCtxIdentSet(val string) {
  v := C.CString(val)
  r := C.MqFactoryCtxIdentSet((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
  this.iErrorMqToGoWithCheck(r)
}

/*****************************************************************************/
/*                                                                           */
/*                                 static                                    */
/*                                                                           */
/*****************************************************************************/

func FactoryAdd(ident string, cb FactoryF) *MqFactoryS {
  v := C.CString(ident)
  r := C.gomsgque_FactoryAdd(v, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(v))
  incrFactoryRef(&cb)
  if (r == nil) {
    panic("MqFactoryS exception");
  }
  return (*MqFactoryS)(r)
}

func FactoryDefault(ident string, cb FactoryF) *MqFactoryS {
  v := C.CString(ident)
  r := C.gomsgque_FactoryDefault(v, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(v))
  incrFactoryRef(&cb)
  if (r == nil) {
    panic("MqFactoryS exception");
  }
  return (*MqFactoryS)(r)
}

func FactoryDefaultIdent() string {
  return C.GoString(C.MqFactoryDefaultIdent())
}

func FactoryGet(ident string) *MqFactoryS {
  v := C.CString(ident)
  r := C.MqFactoryGet(v)
  C.free(unsafe.Pointer(v))
  return (*MqFactoryS)(r)
}

func FactoryGetCalled(ident string) *MqFactoryS {
  v := C.CString(ident)
  r := C.MqFactoryGetCalled(v)
  C.free(unsafe.Pointer(v))
  return (*MqFactoryS)(r)
}


/*****************************************************************************/
/*                                                                           */
/*                                instance                                   */
/*                                                                           */
/*****************************************************************************/

type MqFactoryS _Ctype_struct_MqFactoryS

func (this *MqFactoryS) New() (*MqS) {
  r := C.gomsgque_FactoryNew((*_Ctype_struct_MqFactoryS)(this))
  if (r != nil) {
    return (*MqS)(r)
  }
  panic("MqFactoryS exception");
}

func (this *MqFactoryS) Copy(ident string) (*MqFactoryS) {
  v := C.CString(ident)
  r := (*MqFactoryS)(C.MqFactoryCopy((*_Ctype_struct_MqFactoryS)(this), v))
  C.free(unsafe.Pointer(v))
  return (*MqFactoryS)(r)
}

