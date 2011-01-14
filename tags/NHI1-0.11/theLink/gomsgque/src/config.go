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
#include <gomsgque.h>
*/
import "C"

import (
  //"fmt"
  //"os"
  "unsafe"
)

// global lock for created objects
var ctxlock = make(map[*MqS]interface{})

func (this *MqS) ConfigGetBuffersize() int32 {
  return int32(C.MqConfigGetBuffersize((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetBuffersize(val int32) {
  C.MqConfigSetBuffersize((*_Ctype_struct_MqS)(this), C.MQ_INT(val))
}

type START uint32

const (
  START_DEFAULT  START = C.MQ_START_DEFAULT
  START_FORK	 START = C.MQ_START_FORK
  START_THREAD   START = C.MQ_START_THREAD
  START_SPAWN    START = C.MQ_START_SPAWN
)

func (this *MqS) ConfigGetStartAs() START {
  return START(C.MqConfigGetStartAs((*_Ctype_struct_MqS)(this)))
}

type STATUS_IS uint32

const (
  STATUS_IS_INITIAL STATUS_IS = C.MQ_STATUS_IS_INITIAL
  STATUS_IS_DUP	    STATUS_IS = C.MQ_STATUS_IS_DUP
  STATUS_IS_THREAD  STATUS_IS = C.MQ_STATUS_IS_THREAD
  STATUS_IS_FORK    STATUS_IS = C.MQ_STATUS_IS_FORK
  STATUS_IS_SPAWN   STATUS_IS = C.MQ_STATUS_IS_SPAWN
)

func (this *MqS) ConfigGetStatusIs() STATUS_IS {
  return STATUS_IS(C.MqConfigGetStatusIs((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetStartAs(val START) {
  C.MqConfigSetStartAs((*_Ctype_struct_MqS)(this), uint32(val))
}

func (this *MqS) ConfigGetIoPipeSocket() int32 {
  return int32(C.MqConfigGetIoPipeSocket((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetIoPipeSocket(val int32) {
  C.MqConfigSetIoPipeSocket((*_Ctype_struct_MqS)(this), C.MQ_SOCK(val))
}

func (this *MqS) ConfigGetDebug() int32 {
  return int32(C.MqConfigGetDebug((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetDebug(val int32) {
  C.MqConfigSetDebug((*_Ctype_struct_MqS)(this), C.MQ_INT(val))
}

func (this *MqS) ConfigGetTimeout() int64 {
  return int64(C.MqConfigGetTimeout((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetTimeout(val int64) {
  C.MqConfigSetTimeout((*_Ctype_struct_MqS)(this), C.MQ_TIME_T(val))
}

func (this *MqS) ConfigGetName() string {
  return C.GoString(C.MqConfigGetName((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetName(val string) {
  v := C.CString(val)
  C.MqConfigSetName((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) ConfigGetSrvName() string {
  return C.GoString(C.MqConfigGetSrvName((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetSrvName(val string) {
  v := C.CString(val)
  C.MqConfigSetSrvName((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) ConfigGetIsSilent() bool {
  return C.MqConfigGetIsSilent((*_Ctype_struct_MqS)(this)) == C.MQ_YES
}

func (this *MqS) ConfigSetIsSilent(val bool) {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  C.MqConfigSetIsSilent((*_Ctype_struct_MqS)(this), v)
}

func (this *MqS) ConfigSetIgnoreExit(val bool) {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  C.MqConfigSetIgnoreExit((*_Ctype_struct_MqS)(this), v)
}

func (this *MqS) ConfigGetIsString() bool {
  return C.MqConfigGetIsString((*_Ctype_struct_MqS)(this)) == C.MQ_YES
}

func (this *MqS) ConfigSetIsString(val bool) {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  C.MqConfigSetIsString((*_Ctype_struct_MqS)(this), v)
}

func (this *MqS) ConfigGetIsServer() bool {
  return C.MqConfigGetIsServer((*_Ctype_struct_MqS)(this)) == C.MQ_YES
}

func (this *MqS) ConfigSetIsServer(val bool) {
  var v C.MQ_BOL
  if val {
    v = C.MQ_YES
  } else {
    v = C.MQ_NO
  }
  C.MqConfigSetIsServer((*_Ctype_struct_MqS)(this), v)
}

func (this *MqS) ConfigGetIoUdsFile() string {
  return C.GoString(C.MqConfigGetIoUdsFile((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetIoUdsFile(val string) {
  v := C.CString(val)
  C.MqConfigSetIoUdsFile((*_Ctype_struct_MqS)(this), v)
  C.free(unsafe.Pointer(v))
}

func (this *MqS) ConfigGetIoTcpHost() string {
  return C.GoString(C.MqConfigGetIoTcpHost((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigGetIoTcpPort() string {
  return C.GoString(C.MqConfigGetIoTcpPort((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigGetIoTcpMyHost() string {
  return C.GoString(C.MqConfigGetIoTcpMyHost((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigGetIoTcpMyPort() string {
  return C.GoString(C.MqConfigGetIoTcpMyPort((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetIoTcp(host, port, myhost, myport string) {
  h := C.CString(host)
  p := C.CString(port)
  mh := C.CString(myhost)
  mp := C.CString(myport)
  C.MqConfigSetIoTcp((*_Ctype_struct_MqS)(this), h, p, mh, mp)
  C.free(unsafe.Pointer(mp))
  C.free(unsafe.Pointer(mh))
  C.free(unsafe.Pointer(p))
  C.free(unsafe.Pointer(h))
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "ServerSetup" interfaces
var lockServerSetup  = make(map[*ServerSetup]bool)

type ServerSetup interface {
  ServerSetup()
}

//export cServerSetup
func cServerSetup(this *MqS, cb *ServerSetup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).ServerSetup()
}

//export cServerSetupFree
func cServerSetupFree(cb *ServerSetup) {
  lockServerSetup[cb] = false, false
}

func (this *MqS) ConfigSetServerSetup(cb ServerSetup) {
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
  lockServerSetup[&cb] = true
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "ServerCleanup" interfaces
var lockServerCleanup  = make(map[*ServerCleanup]bool)

type ServerCleanup interface {
  ServerCleanup()
}

//export cServerCleanup
func cServerCleanup(this *MqS, cb *ServerCleanup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).ServerCleanup()
}

//export cServerCleanupFree
func cServerCleanupFree(cb *ServerCleanup) {
  lockServerCleanup[cb] = false, false
}

func (this *MqS) ConfigSetServerCleanup(cb ServerCleanup) {
  C.gomsgque_ConfigSetServerCleanup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
  lockServerCleanup[&cb] = true
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "BgError" interfaces
var lockBgError  = make(map[*BgError]bool)

type BgError interface {
  BgError()
}

//export cBgError
func cBgError(this *MqS, cb *BgError) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).BgError()
}

//export cBgErrorFree
func cBgErrorFree(cb *BgError) {
  lockBgError[cb] = false, false
}

func (this *MqS) ConfigSetBgError(cb BgError) {
  C.gomsgque_ConfigSetBgError((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
  lockBgError[&cb] = true
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// global lock for "Event" interfaces
var lockEvent  = make(map[*Event]bool)

type Event interface {
  Event()
}

//export cEvent
func cEvent(this *MqS, cb *Event) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  (*cb).Event()
}

//export cEventFree
func cEventFree(cb *Event) {
  lockEvent[cb] = false, false
}

func (this *MqS) ConfigSetEvent(cb Event) {
  C.gomsgque_ConfigSetEvent((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
  lockEvent[&cb] = true
}




