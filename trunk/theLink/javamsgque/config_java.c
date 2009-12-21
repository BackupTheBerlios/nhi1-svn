/**
 *  \file       theLink/javamsgque/config_java.c
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

extern jclass	NS(Class_NullPointerException);
extern jfieldID	NS(FID_MqS_hdl);

#define SetC(T) JNIEXPORT void JNICALL NS(T) ( \
  JNIEnv    *env, \
  jobject   self, \
  jstring   strO  \
) \
{ \
  MQ_CST str = JO2C_START(env, strO); \
  Mq ## T (CONTEXT, str); \
  JO2C_STOP(env, strO, str); \
}

#define SetCE(T) JNIEXPORT void JNICALL NS(T) ( \
  JNIEnv    *env, \
  jobject   self, \
  jstring   strO  \
) \
{ \
  SETUP_context; \
  MQ_CST str = JO2C_START(env, strO); \
  ErrorMqToJavaWithCheck (Mq ## T (context, str)); \
  JO2C_STOP(env, strO, str); \
error: \
  return; \
}

#define SetT(T,V) JNIEXPORT void JNICALL NS(V) ( \
  JNIEnv    *env, \
  jobject   self, \
  T         data  \
) \
{ \
  Mq ## V (CONTEXT, data); \
}

#define SetTE(T,V) JNIEXPORT void JNICALL NS(V) ( \
  JNIEnv    *env, \
  jobject   self, \
  T         data  \
) \
{ \
  SETUP_context; \
  ErrorMqToJavaWithCheck (Mq ## V (context, data)); \
error: \
  return; \
}

#define SetI(V)	  SetT(jint,     V)
#define SetIE(V)  SetTE(jint,    V)
#define SetW(V)	  SetT(jlong,    V)
#define SetO(V)	  SetT(jboolean, V)

#define GetC(V) JNIEXPORT jstring JNICALL NS(V) ( \
  JNIEnv    *env, \
  jobject   self  \
) \
{ \
  return JC2O(env,Mq ## V (CONTEXT)); \
}

#define GetT(T,V) JNIEXPORT T JNICALL NS(V) ( \
  JNIEnv    *env, \
  jobject   self  \
) \
{ \
  return Mq ## V (CONTEXT); \
}

#define GetI(V) GetT(jint,     V)
#define GetW(V) GetT(jlong,    V)
#define GetO(V) GetT(jboolean, V)

SetC(ConfigSetName)
SetC(ConfigSetSrvName)
SetC(ConfigSetIdent)
SetI(ConfigSetBuffersize)
SetI(ConfigSetDebug)
SetW(ConfigSetTimeout)
SetO(ConfigSetIsSilent)
SetO(ConfigSetIsServer)
SetO(ConfigSetIsString)

JNIEXPORT jboolean JNICALL NS(ConfigCheckIdent) (
  JNIEnv    *env,
  jobject   self,
  jstring   strO
)
{
  MQ_CST str = JO2C_START(env, strO);
  jboolean check = MqConfigCheckIdent (CONTEXT, str);
  JO2C_STOP(env, strO, str); 
  return check;
}

JNIEXPORT void JNICALL NS(ConfigSetIoTcp) (
  JNIEnv    *env, 
  jobject   self,
  jstring   hostO,
  jstring   portO,
  jstring   myhostO,
  jstring   myportO
)
{
  SETUP_context;
  MQ_CST host,port,myhost,myport;
  host = JO2C_START(env,hostO);
  port = JO2C_START(env,portO);
  myhost = JO2C_START(env,myhostO);
  myport = JO2C_START(env,myportO);
  ErrorMqToJavaWithCheck(MqConfigSetIoTcp (context, host, port, myhost, myport));
error:
  JO2C_STOP(env,hostO,host);
  JO2C_STOP(env,portO,port);
  JO2C_STOP(env,myhostO,myhost);
  JO2C_STOP(env,myportO,myport);
}

GetC(ConfigGetIoTcpHost)
GetC(ConfigGetIoTcpPort)
GetC(ConfigGetIoTcpMyHost)
GetC(ConfigGetIoTcpMyPort)

SetCE(ConfigSetIoUds)
GetC(ConfigGetIoUdsFile)

SetIE(ConfigSetIoPipe)
GetI(ConfigGetIoPipeSocket)

SetI(ConfigSetStartAs)
GetI(ConfigGetStartAs)

GetI(ConfigGetDebug)
GetO(ConfigGetIsString)
GetO(ConfigGetIsSilent)
GetO(ConfigGetIsServer)
GetO(ConfigGetIsParent)
GetO(ConfigGetIsSlave)
GetO(ConfigGetIsConnected)
GetO(ConfigGetIsTransaction)

JNIEXPORT jobject JNICALL NS(ConfigGetParent) (
  JNIEnv    *env, 
  jobject   self
)
{
  return (CONTEXT->config.parent == NULL ? NULL : ((jobject)CONTEXT->config.parent->self));
}

JNIEXPORT jobject JNICALL NS(ConfigGetMaster) (
  JNIEnv    *env, 
  jobject   self
)
{
  return (CONTEXT->config.master == NULL ? NULL : ((jobject)CONTEXT->config.master->self));
}

JNIEXPORT jobject JNICALL NS(ConfigGetFilter__) (
  JNIEnv    *env, 
  jobject   self
)
{
  SETUP_context;
  struct MqS *ftr;
  ErrorMqToJavaWithCheck (MqConfigGetFilter(context, 0, &ftr));
  return ((jobject)ftr->self);
error:
  return NULL;
}

JNIEXPORT jobject JNICALL NS(ConfigGetFilter__I) (
  JNIEnv    *env, 
  jobject   self,
  jint	    id
)
{
  SETUP_context;
  struct MqS *ftr;
  ErrorMqToJavaWithCheck (MqConfigGetFilter(context, id, &ftr));
  return ((jobject)ftr->self);
error:
  return NULL;
}

GetI(ConfigGetBuffersize)
GetW(ConfigGetTimeout)
GetC(ConfigGetName)
GetC(ConfigGetSrvName)
GetC(ConfigGetIdent)

GetI(ConfigGetCtxId)
GetC(ConfigGetToken)

