/**
 *  \file       theLink/javamsgque/send_java.c
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
extern jfieldID     NS(FID_MqBufferS_hdl);
extern jclass	    NS(Class_NullPointerException);
extern jclass	    NS(Class_RuntimeException);
extern jmethodID    NS(MID_ICallback_Callback);

#define MSGQUE msgque

///
///   SEND
///

JNIEXPORT void JNICALL NS(SendSTART) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendSTART(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendEND) (
  JNIEnv *	env, 
  jobject	self,
  jobject	token
)
{
  const char *str;
  SETUP_context;
  str = JO2C_START(env,token);
  ErrorMqToJavaWithCheck(MqSendEND(context,str));
  JO2C_STOP(env,token,str);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendEND_1AND_1WAIT__Ljava_lang_String_2) (
  JNIEnv *	env, 
  jobject	self,
  jobject	token
)
{
  const char *str;
  SETUP_context;
  str = JO2C_START(env,token);
  ErrorMqToJavaWithCheck(MqSendEND_AND_WAIT(context,str,-2));
  JO2C_STOP(env,token,str);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendEND_1AND_1WAIT__Ljava_lang_String_2I) (
  JNIEnv *	env, 
  jobject	self,
  jobject	token,
  jint		timeout
)
{
  const char *str;
  SETUP_context;
  str = JO2C_START(env,token);
  ErrorMqToJavaWithCheck(MqSendEND_AND_WAIT(context,str,timeout));
  JO2C_STOP(env,token,str);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendEND_1AND_1CALLBACK) (
  JNIEnv *	env, 
  jobject	self,
  jstring	token,
  jobject	callback
)
{
  const char *str;
  jobject obj;
  MQ_PTR call;
  SETUP_context;
  str = JO2C_START(env,token);
  JavaErrorCheckNULL(obj = (*env)->NewGlobalRef(env, callback));
  ErrorMqToJavaWithCheck(NS(ProcCreate)(context, obj, NULL, NS(MID_ICallback_Callback), self, &call));
  ErrorMqToJavaWithCheck(MqSendEND_AND_CALLBACK(context, str, NS(ProcCall), call, NS(ProcFree)));
error:
  JO2C_STOP(env,token,str);
  return;
}

JNIEXPORT void JNICALL NS(SendRETURN) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendRETURN(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendL_1START) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendL_START(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendL_1END) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendL_END(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendY) (
  JNIEnv *	env, 
  jobject	self,
  jbyte		y
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendY(context,y));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendO) (
  JNIEnv *	env, 
  jobject	self,
  jboolean	o
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendO(context,(MQ_BOL)o));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendS) (
  JNIEnv *	env, 
  jobject	self,
  jshort	s
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendS(context,s));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendI) (
  JNIEnv *	env, 
  jobject	self,
  jint		i
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendI(context,i));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendF) (
  JNIEnv *	env, 
  jobject	self,
  jfloat	f
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendF(context,f));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendW) (
  JNIEnv *	env, 
  jobject	self,
  jlong		w
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendW(context,(MQ_WID)w));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendD) (
  JNIEnv *	env, 
  jobject	self,
  jdouble	d
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendD(context,d));
error:
  return;
}

JNIEXPORT void JNICALL NS(SendC) (
  JNIEnv *	env, 
  jobject	self,
  jstring	s
)
{
  const char * str;
  SETUP_context;
  str = JO2C_START(env,s);
  ErrorMqToJavaWithCheck(MqSendC(context,str));
  JO2C_STOP(env,s,str);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendB) (
  JNIEnv *	env, 
  jobject	self,
  jbyteArray	b
)
{
  jbyte * tmp;
  SETUP_context;
  tmp = (*env)->GetByteArrayElements(env,b,NULL);
  ErrorMqToJavaWithCheck(MqSendB(context,(MQ_BIN)tmp,(*env)->GetArrayLength(env,b)));
  (*env)->ReleaseByteArrayElements(env,b,tmp,0);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendN) (
  JNIEnv *	env, 
  jobject	self,
  jbyteArray	b
)
{
  jbyte * tmp;
  SETUP_context;
  tmp = (*env)->GetByteArrayElements(env,b,NULL);
  ErrorMqToJavaWithCheck(MqSendN(context,(MQ_BIN)tmp,(*env)->GetArrayLength(env,b)));
  (*env)->ReleaseByteArrayElements(env,b,tmp,0);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendBDY) (
  JNIEnv *	env, 
  jobject	self,
  jbyteArray	b
)
{
  jbyte * tmp;
  SETUP_context;
  tmp = (*env)->GetByteArrayElements(env,b,NULL);
  ErrorMqToJavaWithCheck(MqSendBDY(context,(MQ_BIN)tmp,(*env)->GetArrayLength(env,b)));
  (*env)->ReleaseByteArrayElements(env,b,tmp,0);
error:
  return;
}

JNIEXPORT void JNICALL NS(SendU) (
  JNIEnv *	env, 
  jobject	self,
  jobject	bufO
)
{
  struct MqBufferS * buf = ((struct MqBufferS *) (*env)->GetLongField(env,bufO,NS(FID_MqBufferS_hdl)));
  SETUP_context;
  if (buf == NULL) {
    (*env)->ThrowNew(env, NS(Class_NullPointerException), "javamsgque buffer object already deleted");
    goto error;
  }
  ErrorMqToJavaWithCheck(MqSendU(context,buf));
error:
  return;
}



