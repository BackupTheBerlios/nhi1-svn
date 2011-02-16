/**
 *  \file       theLink/javamsgque/MqDumpS_java.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "context_java.h"
#include "javamsgque_MqDumpS.h"

#define ND(n)	Java_javamsgque_MqDumpS_ ## n

extern jclass	NS(Class_NullPointerException);
extern jfieldID	NS(FID_MqDumpS_hdl);

JNIEXPORT jlong JNICALL ND(DumpFree) (
  JNIEnv *  env, 
  jobject   self
)
{
  SETUP_dump(self);
  MqSysFree(dump);
error:
  return 0L;
}

