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

extern jclass	  NS(Class_NullPointerException);
//extern jclass	  NS(Class_MqS);
extern jfieldID	  NS(FID_MqS_hdl);
//extern jmethodID  NS(MID_MqS_INIT);

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

SetC(ConfigSetName)
SetC(ConfigSetSrvName)
SetC(ConfigSetIdent)
SetI(ConfigSetBuffersize)
SetI(ConfigSetDebug)
SetW(ConfigSetTimeout)
SetO(ConfigSetIsSilent)
SetO(ConfigSetIsServer)
SetO(ConfigSetIsString)
SetO(ConfigSetIgnoreExit)

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

SetCE(ConfigSetIoUdsFile)
GetC(ConfigGetIoUdsFile)

SetIE(ConfigSetIoPipeSocket)
GetI(ConfigGetIoPipeSocket)

GetI(ConfigGetDebug)
GetO(ConfigGetIsString)
GetO(ConfigGetIsSilent)
GetO(ConfigGetIsServer)

GetI(ConfigGetBuffersize)
GetW(ConfigGetTimeout)
GetC(ConfigGetName)
GetC(ConfigGetSrvName)
GetC(ConfigGetIdent)


JNIEXPORT void JNICALL NS(pConfigSetStartAs) (
  JNIEnv    *env,
  jobject   self,
  jint      data
)
{
  MqConfigSetStartAs (CONTEXT, data);
}

JNIEXPORT jint JNICALL NS(pConfigGetStartAs) (
  JNIEnv    *env,
  jobject   self
)
{
  return MqConfigGetStartAs (CONTEXT);
}

/*
JNIEXPORT void JNICALL NS(ConfigSetDefaultFactory) (
  JNIEnv    *env,
  jobject   self,
  jstring   ident
)
{
  const char * str = JO2C_START(env,ident);
  MQ_PTR call = NS(ProcCreate)(env, NULL, NS(Class_MqS), NS(MID_MqS_INIT), NULL);
  JavaErrorCheckNULL(call);
  MqConfigSetFactory(CONTEXT, str, NS(FactoryCreate), call, NS(ProcFree), NS(FactoryDelete), NULL, NULL);
error:
  JO2C_STOP(env,ident,str);
}
*/

