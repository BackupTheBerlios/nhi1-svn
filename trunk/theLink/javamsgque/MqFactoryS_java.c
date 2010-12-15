/**
 *  \file       theLink/javamsgque/context_java.c
 *  \brief      \$Id: context_java.c 324 2010-11-22 11:54:35Z aotto1968 $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 324 $
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MSGQUE msgque

#include "context_java.h"

JNIEXPORT void JNICALL NF(Add) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  MQ_PTR call;
  jmethodID callback;
  const char * str = JO2C_START(env,ident);
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(ProcCreate)(env, NULL, cbClass, callback, NULL));
  MqFactoryAdd(str, NS(FactoryCreate), call, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
error:
  JO2C_STOP(env,ident,str);
}

JNIEXPORT jobject JNICALL NF(Call) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident
)
{
  struct MqS * mqctx;
  const char * str;
  str = JO2C_START(env,ident);
  { 
    struct MqFactoryItemS * item = MqFactoryItemGet(str);
    if (item != NULL && item->Create.fCall != NULL) {
      (*item->Create.fCall) ((struct MqS *)env, MQ_FACTORY_NEW_INIT, item, &mqctx);
    }
  }
  JO2C_STOP(env,ident,str);
  return mqctx ? mqctx->self : NULL;
}

JNIEXPORT jobject JNICALL NF(New) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  JavaErrorCheck(NF(Add) (env, class, ident, cbClass));
  return (*env)->ExceptionCheck(env) != JNI_FALSE ? NULL : NF(Call) (env, class, ident);
error:
  return NULL;
}
