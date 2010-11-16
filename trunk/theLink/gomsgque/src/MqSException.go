/**
 *  \file       theLink/gomsgque/src/MqSException.go
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
#include <msgque.h>
*/
import "C"

import (
  //"fmt"
  //"strings"
  //"unsafe"
)

type MqSException uint32

func (this MqSException) IsERROR() bool {
  return this == ERROR
}


