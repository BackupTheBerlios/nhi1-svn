/**
 *  \file       theLink/javamsgque/read_java.c
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

extern jfieldID	    NS(FID_MqS_hdl);
extern jfieldID	    NS(FID_MqBufferS_hdl);
extern jfieldID	    NS(FID_MqDumpS_hdl);
extern jclass	    NS(Class_NullPointerException);
extern jclass       NS(Class_MqBufferS);
extern jclass       NS(Class_MqDumpS);
extern jmethodID    NS(MID_MqBufferS_INIT);
extern jmethodID    NS(MID_MqDumpS_INIT);

///
///  READ
///

JNIEXPORT void JNICALL NS(ReadL_1START__) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadL_START(context, NULL));
error:
  return;
}

JNIEXPORT void JNICALL NS(ReadL_1START__Ljavamsgque_MqBufferS_2) (
  JNIEnv *	env, 
  jobject	self,
  jobject	bufO
)
{
  SETUP_context;
  struct MqBufferS * buf = ((struct MqBufferS *) (*env)->GetLongField(env,bufO,NS(FID_MqBufferS_hdl)));
  if (buf == NULL) {
    (*env)->ThrowNew(env, NS(Class_NullPointerException), "javamsgque buffer object already deleted");
    goto error;
  }
  ErrorMqToJavaWithCheck(MqReadL_START(context, buf));
error:
  return;
}

JNIEXPORT void JNICALL NS(ReadL_1END) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadL_END(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(ReadT_1START) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadT_START(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(ReadT_1END) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadT_END(context));
error:
  return;
}

JNIEXPORT jbyte JNICALL NS(ReadY) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_BYT i;
  ErrorMqToJavaWithCheck(MqReadY(context, &i));
  return i;
error:
  return 0;
}

JNIEXPORT jboolean JNICALL NS(ReadO) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_BOL i;
  ErrorMqToJavaWithCheck(MqReadO(context, &i));
  return (jboolean)i;
error:
  return 0;
}

JNIEXPORT jshort JNICALL NS(ReadS) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_SRT i;
  ErrorMqToJavaWithCheck(MqReadS(context, &i));
  return i;
error:
  return 0;
}

JNIEXPORT jint JNICALL NS(ReadI) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_INT i;
  ErrorMqToJavaWithCheck(MqReadI(context, &i));
  return i;
error:
  return 0;
}

JNIEXPORT jfloat JNICALL NS(ReadF) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_FLT i;
  ErrorMqToJavaWithCheck(MqReadF(context, &i));
  return i;
error:
  return 0;
}

JNIEXPORT jlong JNICALL NS(ReadW) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_WID w;
  ErrorMqToJavaWithCheck(MqReadW(context, &w));
  return w;
error:
  return 0L;
}

JNIEXPORT jdouble JNICALL NS(ReadD) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_DBL d;
  ErrorMqToJavaWithCheck(MqReadD(context, &d));
  return d;
error:
  return 0.0;
}

JNIEXPORT jstring JNICALL NS(ReadC) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  MQ_CST s = NULL;
  ErrorMqToJavaWithCheck(MqReadC(context, &s));
  return JC2O(env,s);
error:
  return NULL;
}

JNIEXPORT jbyteArray JNICALL NS(ReadB) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  MQ_BIN b;
  MQ_SIZE len;
  jbyteArray tmp;
  ErrorMqToJavaWithCheck(MqReadB(context, &b, &len));
  tmp = (*env)->NewByteArray(env,len);
  (*env)->SetByteArrayRegion(env,tmp,0,len,(jbyte*)b);
  return tmp;
error:
  return NULL;
}

JNIEXPORT jbyteArray JNICALL NS(ReadN) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  MQ_CBI b;
  MQ_SIZE len;
  jbyteArray tmp;
  ErrorMqToJavaWithCheck(MqReadN(context, &b, &len));
  tmp = (*env)->NewByteArray(env,len);
  (*env)->SetByteArrayRegion(env,tmp,0,len,(jbyte*)b);
  return tmp;
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NS(ReadDUMP) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  struct MqDumpS *bdy;
  ErrorMqToJavaWithCheck(MqReadDUMP(context, &bdy));
  return (*env)->NewObject(env, NS(Class_MqDumpS), NS(MID_MqDumpS_INIT), (jlong) bdy);
error:
  return NULL;
}

JNIEXPORT void JNICALL NS(ReadLOAD) (
  JNIEnv *	env, 
  jobject	self,
  jobject	dumpO
)
{
  SETUP_context;
  SETUP_dump(dumpO);
  ErrorMqToJavaWithCheck(MqReadLOAD(context,dump));
error:
  return;
}

JNIEXPORT jobject JNICALL NS(ReadU) (
  JNIEnv *      env,
  jobject       self
)
{
  SETUP_context;
  MQ_BUF buffer;
  ErrorMqToJavaWithCheck(MqReadU(context, &buffer));
  return (*env)->NewObject(env, NS(Class_MqBufferS), NS(MID_MqBufferS_INIT), (jlong) buffer);
error:
  return NULL;
}

JNIEXPORT void JNICALL NS(ReadProxy) (
  JNIEnv *      env,
  jobject       self,
  jobject	ctx
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadProxy(context, XCONTEXT(ctx)));
error:
  return;
}

JNIEXPORT void JNICALL NS(ReadForward) (
  JNIEnv *      env,
  jobject       self,
  jobject	ctx
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadForward(context, XCONTEXT(ctx)));
error:
  return;
}

JNIEXPORT jint JNICALL NS(ReadGetNumItems) (
  JNIEnv *	env, 
  jobject	self
)
{
  return MqReadGetNumItems(CONTEXT);
}

JNIEXPORT jboolean JNICALL NS(ReadItemExists) (
  JNIEnv *	env, 
  jobject	self
)
{
  return (MqReadItemExists(CONTEXT) ? JNI_TRUE : JNI_FALSE);
}

JNIEXPORT void JNICALL NS(ReadUndo) (
  JNIEnv *	env, 
  jobject	self
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqReadUndo(context));
error:
  return;
}




