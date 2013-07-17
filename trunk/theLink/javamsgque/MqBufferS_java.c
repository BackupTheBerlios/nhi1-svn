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

#define GET(T,L,J) \
JNIEXPORT J JNICALL NB(Get ## T) ( \
  JNIEnv    *env, \
  jobject   self \
) \
{ \
  L v; \
  SETUP_buf(self); \
  ErrorMqBufferToJavaWithCheck(MqBufferGet ## T(buf,&v)); \
  return (J) v; \
error: \
  return (J) 0; \
}

GET(Y,MQ_BYT,jbyte)
GET(O,MQ_BOL,jboolean)
GET(S,MQ_SRT,jshort)
GET(I,MQ_INT,jint)
GET(F,MQ_FLT,jfloat)
GET(W,MQ_WID,jlong)
GET(D,MQ_DBL,jdouble)

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

#define SET(T,L,J) \
JNIEXPORT jobject JNICALL NB(Set ## T) ( \
  JNIEnv *      env, \
  jobject       self, \
  J		val \
) \
{ \
  SETUP_buf(self); \
  MqBufferSet ## T (buf, (L) val); \
  return self; \
error: \
  return NULL; \
}

SET(Y,MQ_BYT,jbyte)
SET(O,MQ_BOL,jboolean)
SET(S,MQ_SRT,jshort)
SET(I,MQ_INT,jint)
SET(F,MQ_FLT,jfloat)
SET(W,MQ_WID,jlong)
SET(D,MQ_DBL,jdouble)

JNIEXPORT jobject JNICALL NB(SetC) (
  JNIEnv *	env, 
  jobject	self,
  jstring	s
)
{
  const char * str;
  MQ_BUF ret;
  SETUP_buf(self);
  str = JO2C_START(env,s);
  ret = MqBufferSetC(buf,str);
  JO2C_STOP(env,s,str);
  return self;
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NB(SetB) (
  JNIEnv *	env, 
  jobject	self,
  jbyteArray	b
)
{
  jbyte * tmp;
  MQ_BUF ret;
  SETUP_buf(self);
  tmp = (*env)->GetByteArrayElements(env,b,NULL);
  ret = MqBufferSetB(buf,(MQ_BIN)tmp,(*env)->GetArrayLength(env,b));
  (*env)->ReleaseByteArrayElements(env,b,tmp,0);
  return self;
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NB(AppendC) (
  JNIEnv *	env, 
  jobject	self,
  jstring	s
)
{
  const char * str;
  MQ_BUF ret;
  SETUP_buf(self);
  str = JO2C_START(env,s);
  ret = MqBufferAppendC(buf,str);
  JO2C_STOP(env,s,str);
  return self;
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

