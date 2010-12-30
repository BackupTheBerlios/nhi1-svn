/**
 *  \file       theLink/javamsgque/MqFactoryS_java.c
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#define MSGQUE msgque

#include "context_java.h"
#include "javamsgque_MqFactoryS.h"

#define NF(n)	Java_javamsgque_MqFactoryS_ ## n

JNIEXPORT void JNICALL NF(Add) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_ADD_ERR;
  MQ_PTR call;
  jmethodID callback;
  const char * str;
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(ProcCreate)(env, NULL, cbClass, callback, NULL));
  str = JO2C_START(env,ident);
  ret = MqFactoryAdd(str, NS(FactoryCreate), call, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  JO2C_STOP(env,ident,str);
  MqFactoryErrorCheck(ret);
  return;
error:
  ErrorStringToJava(MqFactoryErrorMsg(ret));
  return;  
}

JNIEXPORT void JNICALL NF(Default) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_DEFAULT_ERR;
  MQ_PTR call;
  jmethodID callback;
  const char * str;
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(ProcCreate)(env, NULL, cbClass, callback, NULL));
  str = JO2C_START(env,ident);
  ret = MqFactoryDefault(str, NS(FactoryCreate), call, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
  JO2C_STOP(env,ident,str);
  MqFactoryErrorCheck(ret);
  return;
error:
  ErrorStringToJava(MqFactoryErrorMsg(ret));
  return;  
}

JNIEXPORT jstring JNICALL NF(DefaultIdent) (
  JNIEnv  *env, 
  jclass  class
)
{
  MQ_CST str = MqFactoryDefaultIdent();
  return JC2O(env,str);
}

JNIEXPORT jobject JNICALL NF(Call) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident
)
{
  struct MqS * mqctx;
  const char * str;
  enum MqFactoryReturnE ret;
  str = JO2C_START(env,ident);
  ret = MqFactoryCall (str, (MQ_PTR)env, &mqctx);
  JO2C_STOP(env,ident,str);
  MqFactoryErrorCheck(ret);
  return mqctx->self;
error:
  ErrorStringToJava(MqFactoryErrorMsg(ret));
  return NULL;  
}

JNIEXPORT jobject JNICALL NF(New) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  MQ_PTR call;
  jmethodID callback;
  struct MqS * mqctx;
  const char * str;
  enum MqFactoryReturnE ret = MQ_FACTORY_RETURN_NEW_ERR;
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(ProcCreate)(env, NULL, cbClass, callback, NULL));
  str = JO2C_START(env,ident);
  ret = MqFactoryNew (str, NS(FactoryCreate), call, NS(ProcFree), NS(FactoryDelete), NULL, NULL, (MQ_PTR)env, &mqctx);
  JO2C_STOP(env,ident,str);
  MqFactoryErrorCheck(ret);
  return mqctx->self;
error:
  ErrorStringToJava(MqFactoryErrorMsg(ret));
  return NULL;  
}
