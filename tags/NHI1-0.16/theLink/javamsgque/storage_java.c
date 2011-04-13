/**
 *  \file       theLink/javamsgque/storage_java.c
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

extern jfieldID	    NS(FID_MqS_hdl);
//extern jfieldID     NS(FID_MqBufferS_hdl);
//extern jclass	    NS(Class_NullPointerException);
//extern jclass	    NS(Class_RuntimeException);
//extern jmethodID    NS(MID_ICallback_Callback);

#define MSGQUE msgque

JNIEXPORT void JNICALL NS(StorageOpen) (
  JNIEnv *	env, 
  jobject	self, 
  jstring	storageFile
)
{
  const char * str;
  enum MqErrorE ret;
  SETUP_context;
  str = JO2C_START(env,storageFile);
  ret = MqStorageOpen(context, str);
  JO2C_STOP(env,storageFile,str);
  ErrorMqToJavaWithCheck(ret);
error:
  return;
}

JNIEXPORT void JNICALL NS(StorageClose) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageClose(context));
error:
  return;
}

JNIEXPORT jlong JNICALL NS(StorageInsert) (
  JNIEnv *	env, 
  jobject	self
)
{
  MQ_TRA transLId;
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageInsert(context, &transLId));
  return transLId;
error:
  return 0LL;
}

JNIEXPORT jlong JNICALL NS(StorageSelect__) (
  JNIEnv *	env, 
  jobject	self
)
{
  MQ_TRA transLId = 0LL;
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageSelect(context, &transLId));
  return transLId;
error:
  return 0LL;
}

JNIEXPORT jlong JNICALL NS(StorageSelect__J) (
  JNIEnv *	env, 
  jobject	self,
  jlong		transLId
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageSelect(context, (MQ_TRA*) &transLId));
  return transLId;
error:
  return 0LL;
}

JNIEXPORT void JNICALL NS(StorageDelete) (
  JNIEnv *	env, 
  jobject	self,
  jlong		transLId
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageDelete(context, transLId));
error:
  return;
}

JNIEXPORT jlong JNICALL NS(StorageCount) (
  JNIEnv *	env, 
  jobject	self
)
{
  MQ_TRA count = 0LL;
  SETUP_context;
  ErrorMqToJavaWithCheck(MqStorageCount(context, &count));
  return count;
error:
  return 0LL;
}
