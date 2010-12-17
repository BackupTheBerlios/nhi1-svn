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
  "os"
  "runtime"
  //"time"
)

func init() {
  runtime.GOMAXPROCS(2)
  C.gomsgque_Init()
}

type TIMEOUT  C.MQ_TIME_T
type WAIT     uint32

const (
  TIMEOUT_DEFAULT TIMEOUT	= C.MQ_TIMEOUT_DEFAULT
  TIMEOUT_USER    TIMEOUT	= C.MQ_TIMEOUT_USER
  TIMEOUT_MAX     TIMEOUT	= C.MQ_TIMEOUT_MAX

  WAIT_NO	  WAIT	= C.MQ_WAIT_NO
  WAIT_ONCE	  WAIT	= C.MQ_WAIT_ONCE
  WAIT_FOREVER    WAIT	= C.MQ_WAIT_FOREVER
)

type MqS _Ctype_struct_MqS

// for toplevel "*MqS"
func NewMqS(tmpl *MqS, ifc interface{}) *MqS {
  ctx := C.MqContextCreate(0,(*_Ctype_struct_MqS)(tmpl))
  ret := (*MqS)(ctx)
  // cleanup "lock" after "object" is deleted
  C.gomsgque_ConfigSetFactory(ctx, C.sDEFAULT, nil)
  // set default action for startup (check for "left over arguments")
  C.gomsgque_ConfigSetSetup(ctx)
  // save the pointer, use "SetSelf" to create link to toplevel object
  // save the pointer, use "SetSelf" to create link to toplevel object
  ret.SetSelf(ifc)
  return ret
}

// for embedding "MqS"
func (this *MqS) Init() {
  C.MqContextInit((*_Ctype_struct_MqS)(this), 0,nil)
}

func (this *MqS) Delete() {
  ctxlock[this] = nil, false
  C.MqContextFree((*_Ctype_struct_MqS)(this))
}

// set link between *MqS and toplevel object
func (this *MqS) SetSelf(ifc interface{}) {
  ctxlock[this] = ifc
  // add interfaces based callback's
  if ifc != nil {
    if obj,ok := ifc.(ServerSetup); ok {
      this.ConfigSetServerSetup(obj)
    }
    if obj,ok := ifc.(ServerCleanup); ok {
      this.ConfigSetServerCleanup(obj)
    }
    if obj,ok := ifc.(BgError); ok {
      this.ConfigSetBgError(obj)
    }
    if obj,ok := ifc.(Event); ok {
      this.ConfigSetEvent(obj)
    }
  }
}

// get toplevel object from *MqS
func (this *MqS) GetSelf() interface{} {
  return ctxlock[this]
}

func (this *MqS) LogC(prefix string, level int32, message string) {
  p := C.CString(prefix)
  m := C.CString(message)
  C.MqLogC((*_Ctype_struct_MqS)(this), p, C.MQ_INT(level), m)
  C.free(unsafe.Pointer(m))
  C.free(unsafe.Pointer(p))
}

func (this *MqS) Exit() {
  C.MqExitP(C.sGO, (*_Ctype_struct_MqS)(this))
}

// global lock for thread-channel objects
// this lock is used by the thread-starter to wait for the exit of the thread-started
var lockThread = make(map[*chan bool]bool)

//export gomsgque_CreateThread
func gomsgque_CreateThread (data *_Ctype_struct_MqSysServerThreadMainS) *chan bool {
  chn := make(chan bool)
  lockThread[&chn] = true
  go func() {
    runtime.LockOSThread()
    C.gomsgque_SysServerThreadMain(data, C.MQ_PTR(&chn))
  }()
  return &chn
}

//export gomsgque_WaitForThread
func gomsgque_WaitForThread(chnp *chan bool) {
  <-(*chnp)
  lockThread[chnp] = false, false
}

//export gomsgque_ProcessExit
func gomsgque_ProcessExit (num int32) {
  os.Exit(int(num))
}

//export gomsgque_ThreadExit
func gomsgque_ThreadExit (num int32) {
  runtime.Goexit()
}

//
// ===========================================================
// static functions (namespace: gomsgque)
//

func Init(argv ... string) {
  initB := C.MqInitCreate()
  for _,arg := range argv {
      s := C.CString(arg)
    C.MqBufferLAppendC(initB, s)
      C.free(unsafe.Pointer(s))
  }
}

/*
func (this *MqS) Dummy(i int32) *MqS {
  var ret *_Ctype_struct_MqS
  this.iErrorMqToGoWithCheck(C.MqDummy((*_Ctype_struct_MqS)(this), C.MQ_SIZE(i), &ret))
  return (*MqS)(ret)
}
*/

