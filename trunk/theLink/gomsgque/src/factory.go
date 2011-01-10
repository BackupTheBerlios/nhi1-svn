/**
 *  \file       theLink/gomsgque/src/factory.go
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
var lockFactory  = make(map[*FactoryF]bool)

// factory erro-handling
func iErrorFactoryToGoWithCheck(ret uint32) {
  if ret != C.MQ_FACTORY_RETURN_OK {
    panic(C.GoString(C.MqFactoryErrorMsg(ret)));
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
  ctxlock[this] = nil, false
  if chnp != nil {
    (*chnp) <- true
  }
}

//export cFactoryFree
func cFactoryFree(cb *FactoryF) {
  lockFactory[cb] = false, false
}

/*****************************************************************************/
/*                                                                           */
/*                                 context                                   */
/*                                                                           */
/*****************************************************************************/

func (this *MqS) FactoryCtxIdentGet() string {
  return C.GoString(C.MqFactoryCtxIdentGet((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) FactoryCtxIdentSet(val string) {
  v := C.CString(val)
  r := C.MqFactoryCtxIdentSet((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
  this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) FactoryCtxDefaultSet(ident string) {
  v := C.CString(ident)
  r := C.MqFactoryCtxDefaultSet((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
  this.iErrorMqToGoWithCheck(r)
}

/*****************************************************************************/
/*                                                                           */
/*                                 static                                    */
/*                                                                           */
/*****************************************************************************/

func FactoryAdd(ident string, cb FactoryF) {
  v := C.CString(ident)
  r := C.gomsgque_FactoryAdd(v, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(v))
  iErrorFactoryToGoWithCheck(r)
  lockFactory[&cb] = true
}

/*
func FactoryDefault(ident string, cb FactoryF) {
  v := C.CString(ident)
  r := C.gomsgque_FactoryDefault(v, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(v))
  this.iErrorFactoryToGoWithCheck(r)
  lockFactory[&cb] = true
}
*/

func FactoryDefaultIdent() string {
  return C.GoString(C.MqFactoryDefaultIdent())
}

func FactoryCall(ident string) (*MqS) {
  v := C.CString(ident)
  out := C.gomsgque_FactoryCall(v)
  C.free(unsafe.Pointer(v))
  iErrorFactoryToGoWithCheck(out.ret)
  return (*MqS)(out.ctx)
}

func FactoryNew(ident string, cb FactoryF) *MqS {
  v := C.CString(ident)
  out := C.gomsgque_FactoryNew(v, C.MQ_PTR(&cb))
  C.free(unsafe.Pointer(v))
  iErrorFactoryToGoWithCheck(out.ret)
  lockFactory[&cb] = true
  return (*MqS)(out.ctx)
}




