/**
 *  \file       theLink/gomsgque/src/MqS.go
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
  "unsafe"
  //"reflect"
)

type TIMEOUT  C.MQ_TIME_T
type WAIT     uint32

const (
  MqS_TIMEOUT_DEFAULT TIMEOUT	= C.MQ_TIMEOUT_DEFAULT
  MqS_TIMEOUT_USER    TIMEOUT	= C.MQ_TIMEOUT_USER
  MqS_TIMEOUT_MAX     TIMEOUT	= C.MQ_TIMEOUT_MAX

  MqS_WAIT_NO	      WAIT	= C.MQ_WAIT_NO
  MqS_WAIT_ONCE	      WAIT	= C.MQ_WAIT_ONCE
  MqS_WAIT_FOREVER    WAIT	= C.MQ_WAIT_FOREVER
)

type MqS _Ctype_struct_MqS

// for toplevel "*MqS"
func NewMqS(ifc interface{}) *MqS {
  ctx := C.MqContextCreate(0,nil)
  ret := (*MqS)(ctx)
  // save the pointer, use "SetSelf" to create link to toplevel object
  lock[ret] = ifc
//fmt.Printf("NewMqS => c:%p -> i:%p\n", ret, ifc)
  // cleanup "lock" after "object" is deleted
  C.gomsgque_ConfigSetFactory(ctx, nil)
  // set default action for startup (check for "left over arguments")
  C.gomsgque_ConfigSetSetup(ctx)
  // no THREAD
  C.MqConfigSetIgnoreThread(ctx, C.MQ_YES)
  // no FORK
  //C.MqConfigSetIgnoreFork(ctx, C.MQ_YES)
  ret.ConfigSetStartAs(MQ_START_FORK)
  // add interfaces
  if ifc != nil {
    if obj,ok := ifc.(ServerSetup); ok {
      ret.ConfigSetServerSetup(obj)
    }
    if obj,ok := ifc.(ServerCleanup); ok {
      ret.ConfigSetServerCleanup(obj)
    }
    if obj,ok := ifc.(Factory); ok {
      ret.ConfigSetFactory(obj)
    }
    if obj,ok := ifc.(BgError); ok {
      ret.ConfigSetBgError(obj)
    }
    if obj,ok := ifc.(Event); ok {
      ret.ConfigSetEvent(obj)
    }
  }
  return ret
}

// for embedding "MqS"
func (this *MqS) Init() {
  C.MqContextInit((*_Ctype_struct_MqS)(this), 0,nil)
}

// set link between *MqS and toplevel object
func (this *MqS) SetSelf(ifc interface{}) {
//fmt.Printf("SetSelf => c:%p -> i:%p\n", this, ifc)
  lock[this] = ifc
}

// get toplevel object from *MqS
func (this *MqS) GetSelf() interface{} {
//fmt.Printf("GetSelf => c:%p\n", this)
  return lock[this]
}

func (this *MqS) LogC(prefix string, level int, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC((*_Ctype_struct_MqS)(this), p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(m))
  C.free(unsafe.Pointer(p))
}

func (this *MqS) Exit() {
  C.MqExitP(C.sGO, (*_Ctype_struct_MqS)(this))
}

func (this *MqS) SysSleep(sec uint32) {
  this.iErrorMqToGoWithCheck(C.MqSysSleep((*_Ctype_struct_MqS)(this), C.uint(sec)))
}

func (this *MqS) SysUSleep(sec uint32) {
  this.iErrorMqToGoWithCheck(C.MqSysUSleep((*_Ctype_struct_MqS)(this), C.uint(sec)))
}

func Init(argv []string) {
  initB := C.MqInitCreate()
  for _,arg := range argv {
      s := C.CString(arg)
    C.MqBufferLAppendC(initB, s)
      C.free(unsafe.Pointer(s))
  }
}

