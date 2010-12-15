/**
 *  \file       theLink/javamsgque/service_java.c
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
extern jmethodID    NS(MID_IService_Service);
extern jclass	    NS(Class_NullPointerException);

GetC(ServiceGetToken)
GetO(ServiceIsTransaction)

JNIEXPORT jobject JNICALL NS(ServiceGetFilter__) (
  JNIEnv    *env, 
  jobject   self
)
{
  SETUP_context;
  struct MqS *ftr;
  ErrorMqToJavaWithCheck (MqServiceGetFilter(context, 0, &ftr));
  return ((jobject)ftr->self);
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NS(ServiceGetFilter__I) (
  JNIEnv    *env, 
  jobject   self,
  jint	    id
)
{
  SETUP_context;
  struct MqS *ftr;
  ErrorMqToJavaWithCheck (MqServiceGetFilter(context, id, &ftr));
  return ((jobject)ftr->self);
error:
  return NULL;
}

JNIEXPORT void JNICALL NS(ServiceCreate) (
  JNIEnv *  env, 
  jobject   self, 
  jstring   token,
  jobject   service
)
{
  SETUP_context;
  MQ_PTR call;
  const char *tokenC = JO2C_START(env, token);

  JavaErrorCheckNULL(
    call = NS(ProcCreate)(env, service, (*env)->GetObjectClass(env, service), NS(MID_IService_Service), context->self)
  );
  ErrorMqToJavaWithCheck(MqServiceCreate(context, tokenC, NS(ProcCall), call, NS(ProcFree)));
error:
  JO2C_STOP(env, token, tokenC);
  return;
}

JNIEXPORT void JNICALL NS(ServiceDelete) (
  JNIEnv *  env, 
  jobject   self, 
  jstring   token
)
{
  SETUP_context;
  const char *tokenC = JO2C_START(env, token);
  ErrorMqToJavaWithCheck(MqServiceDelete(context, tokenC));
  JO2C_STOP(env, token, tokenC);
error:
  return;
}

JNIEXPORT void JNICALL NS(ServiceProxy__Ljava_lang_String_2) (
  JNIEnv *  env, 
  jobject   self, 
  jstring   token
)
{
  SETUP_context;
  const char *tokenC = JO2C_START(env, token);
  ErrorMqToJavaWithCheck(MqServiceProxy(context, tokenC, 0));
  JO2C_STOP(env, token, tokenC);
error:
  return;
}

JNIEXPORT void JNICALL NS(ServiceProxy__Ljava_lang_String_2I) (
  JNIEnv *  env, 
  jobject   self, 
  jstring   token,
  jint	    id
)
{
  SETUP_context;
  const char *tokenC = JO2C_START(env, token);
  ErrorMqToJavaWithCheck(MqServiceProxy(context, tokenC, id));
  JO2C_STOP(env, token, tokenC);
error:
  return;
}

