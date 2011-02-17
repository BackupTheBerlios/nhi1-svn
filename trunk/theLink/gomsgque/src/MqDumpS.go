/**
 */

package gomsgque

/*
#include <gomsgque.h>
*/
import "C"

import (
  //  "unsafe"
)

type MqDumpS _Ctype_struct_MqDumpS

func (this *MqDumpS) Size() int32 {
  return int32(C.MqDumpSize((*_Ctype_struct_MqDumpS)(this)))
}

func (this *MqDumpS) Delete() {
  C.MqSysFreeP(C.MQ_PTR(this))
}
