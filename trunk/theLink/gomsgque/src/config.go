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
  "unsafe"
)

func (this *MqS) ConfigGetBuffersize() int32 {
  return int32(C.MqConfigGetBuffersize((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetBuffersize(val int32) {
  C.MqConfigSetBuffersize((*_Ctype_struct_MqS)(this), C.MQ_INT(val))
}

type MqStartE uint32

const (
  MQ_START_DEFAULT  MqStartE = C.MQ_START_DEFAULT
  MQ_START_FORK	    MqStartE = C.MQ_START_FORK
  MQ_START_THREAD   MqStartE = C.MQ_START_THREAD
  MQ_START_SPAWN    MqStartE = C.MQ_START_SPAWN
)

func (this *MqS) ConfigGetStartAs() MqStartE {
  return MqStartE(C.MqConfigGetStartAs((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetStartAs(val MqStartE) {
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

func (this *MqS) ConfigGetIdent() string {
  return C.GoString(C.MqConfigGetIdent((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) ConfigSetIdent(val string) {
  v := C.CString(val)
  C.MqConfigSetIdent((*_Ctype_struct_MqS)(this), v)
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

//export gomsgque_cServerSetup
func (this *MqS) cServerSetup(cb ServerSetup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  cb.ServerSetup(this)
}

func (this *MqS) ConfigSetServerSetup(cb ServerSetup) {
  C.gomsgque_ConfigSetServerSetup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

//export gomsgque_cServerCleanup
func (this *MqS) cServerCleanup(cb ServerCleanup) {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  cb.ServerCleanup(this)
}

func (this *MqS) ConfigSetServerCleanup(cb ServerCleanup) {
  C.gomsgque_ConfigSetServerCleanup((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

//export gomsgque_cFactoryCreate
func (this *MqS) cFactoryCreate(cb Factory) *MqS {
  defer func() {
    if x := recover(); x != nil {
      this.ErrorSet(x)
    }
  }()
  return cb.Factory(this)
}

func (this *MqS) ConfigSetFactory(cb Factory) {
  C.gomsgque_ConfigSetFactory((*_Ctype_struct_MqS)(this), unsafe.Pointer(&cb))
}

