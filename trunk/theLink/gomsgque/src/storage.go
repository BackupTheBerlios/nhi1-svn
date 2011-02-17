/**
 */

package gomsgque

/*
#include <gomsgque.h>
*/
import "C"

import (
  //"fmt"
  "unsafe"
  //"os"
  //"runtime"
  //"time"
)

func (this *MqS) StorageOpen (storageFile string) {
  s := C.CString(storageFile)
  ret := C.MqStorageOpen((*_Ctype_struct_MqS)(this),s)
  C.free(unsafe.Pointer(s))
  this.iErrorMqToGoWithCheck(ret)
}

func (this *MqS) StorageClose () {
  this.iErrorMqToGoWithCheck(C.MqStorageClose((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) StorageInsert () (transLId int64) {
  this.iErrorMqToGoWithCheck(C.MqStorageInsert((*_Ctype_struct_MqS)(this), (*C.MQ_TRA)(&transLId)))
  return
}

func (this *MqS) StorageSelect () int64 {
  var transLId C.MQ_TRA
  this.iErrorMqToGoWithCheck(C.MqStorageSelect((*_Ctype_struct_MqS)(this), &transLId))
  return int64(transLId)
}

func (this *MqS) StorageSelect2 (transLId int64) int64 {
  this.iErrorMqToGoWithCheck(C.MqStorageSelect((*_Ctype_struct_MqS)(this), (*C.MQ_TRA)(&transLId)))
  return transLId
}

func (this *MqS) StorageDelete (transLId int64) {
  this.iErrorMqToGoWithCheck(C.MqStorageDelete((*_Ctype_struct_MqS)(this), (C.MQ_TRA)(transLId)))
}

func (this *MqS) StorageCount () (count int64) {
  this.iErrorMqToGoWithCheck(C.MqStorageCount((*_Ctype_struct_MqS)(this), (*C.MQ_TRA)(&count)))
  return
}
