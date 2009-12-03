/**
 *  \file       theLink/javamsgque/service_java.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "context_java.h"

extern jfieldID	    NS(FID_MqS_hdl);
extern jmethodID    NS(MID_IService_Service);
extern jclass	    NS(Class_NullPointerException);

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

  ErrorMqToJavaWithCheck(
    NS(ProcCreate)(context, service, (*env)->GetObjectClass(env, service), NS(MID_IService_Service), context->self, &call)
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



