/**
 *  \file       theLink/javamsgque/MqBufferS_java.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "context_java.h"
#include "javamsgque_MqBufferS.h"

#define NB(n)	Java_javamsgque_MqBufferS_ ## n

extern jclass	  NS(Class_NullPointerException);
extern jfieldID	  NS(FID_MqBufferS_hdl);
extern jclass	  NS(Class_MqBufferS);
extern jmethodID  NS(MID_MqBufferS_INIT);

JNIEXPORT jchar JNICALL NB(GetType) (
  JNIEnv *  env, 
  jobject   self
)
{
  SETUP_buf(self);
  return MqBufferGetType(buf);
error:
  return 0;
}

JNIEXPORT jbyte JNICALL NB(GetY) (
  JNIEnv    *env, 
  jobject   self
)
{
  MQ_BYT v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetY(buf,&v));
  return (jbyte) v;
error:
  return 0;
}

JNIEXPORT jboolean JNICALL NB(GetO) (
  JNIEnv    *env, 
  jobject   self
)
{
  MQ_BOL v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetO(buf,&v));
  return (jboolean) v;
error:
  return 0;
}

JNIEXPORT jshort JNICALL NB(GetS) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_SRT v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetS(buf,&v));
  return (jshort) v;
error:
  return 0;
}

JNIEXPORT jint JNICALL NB(GetI) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_INT v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetI(buf,&v));
  return (jint) v;
error:
  return 0;
}

JNIEXPORT jfloat JNICALL NB(GetF) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_FLT v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetF(buf,&v));
  return (jfloat) v;
error:
  return 0;
}

JNIEXPORT jlong JNICALL NB(GetW) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_WID v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetW(buf,&v));
  return (jlong) v;
error:
  return 0L;
}

JNIEXPORT jdouble JNICALL NB(GetD) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_DBL v;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetD(buf,&v));
  return (jdouble) v;
error:
  return 0.0;
}

JNIEXPORT jobject JNICALL NB(GetC) (
  JNIEnv    *env, 
  jobject   self 
)
{
  MQ_CST v = NULL;
  SETUP_buf(self);
  ErrorMqBufferToJavaWithCheck(MqBufferGetC(buf,&v));
  return JC2O(env,v);
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NB(GetB) (
  JNIEnv    *env, 
  jobject   self 
)
{
  jbyteArray tmp;
  SETUP_buf(self);
  tmp = (*env)->NewByteArray(env,buf->cursize);
  (*env)->SetByteArrayRegion(env,tmp,0,buf->cursize,(jbyte*)buf->data);
  return tmp;
error:
  return NULL;
}

JNIEXPORT void JNICALL NB(Delete) (
  JNIEnv    *env, 
  jobject   self 
)
{
  SETUP_buf(self);
  MqBufferDelete((struct MqBufferS **)&buf);
  (*env)->SetLongField(env,self,NS(FID_MqBufferS_hdl), 0L);
error:
  return;
}

JNIEXPORT jobject JNICALL NB(Dup) (
  JNIEnv    *env, 
  jobject   self 
)
{
  SETUP_buf(self);
  return (*env)->NewObject(env, NS(Class_MqBufferS), NS(MID_MqBufferS_INIT), (jlong) MqBufferDup(buf));
error:
  return NULL;
}

