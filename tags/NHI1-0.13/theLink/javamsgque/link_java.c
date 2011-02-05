/**
 *  \file       theLink/javamsgque/link_java.c
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

extern jfieldID	NS(FID_MqS_hdl);

GetO(LinkIsParent)
GetO(LinkIsConnected)
GetI(LinkGetCtxId)

JNIEXPORT jobject JNICALL NS(LinkGetParent) (
  JNIEnv    *env, 
  jobject   self
)
{
  struct MqS * const parent = MqLinkGetParent(CONTEXT);
  return (parent != NULL ? ((jobject)parent->self) : NULL );
}

JNIEXPORT void JNICALL NS(LinkCreate) (
  JNIEnv *	env, 
  jobject	self, 
  jobjectArray	argv
)
{
  SETUP_context;
  struct MqBufferLS * largv = NULL;
  jsize argc = (*env)->GetArrayLength(env, argv);

  // fill the argv/alfa
  if (argv != NULL && argc > 0) {
    jobject obj;
    const char *str;
    int i;
    largv = MqBufferLCreate (argc+1);
    for (i=0; i<argc; i++) {
      obj = (*env)->GetObjectArrayElement(env, argv, i);
      str = JO2C_START(env, obj);
      if (i==0 && (str[0] == '-' || str[0] == MQ_ALFA)) {
	MqBufferLAppendC(largv,context->config.name == NULL ? "java" : context->config.name);
      }
      MqBufferLAppendC(largv,str);
      JO2C_STOP(env,obj,str);
    }
  }

  // create msgque object
  ErrorMqToJavaWithCheck (MqLinkCreate (context, &largv));
error:
  return;
}

JNIEXPORT void JNICALL NS(LinkCreateChild) (
  JNIEnv *	env, 
  jobject	self, 
  jobject	parentO, 
  jobjectArray	argv
)
{
  SETUP_context;
  struct MqS * const parent = (parentO == NULL ? NULL : XCONTEXT(parentO));
  struct MqBufferLS * largv = NULL;

  // fill the argv/alfa
  if (argv != NULL) {
    jsize argc = (*env)->GetArrayLength(env, argv);
    jsize i;
    jobject obj;
    const char *str;
    largv = MqBufferLCreate (argc+1);
    for (i=0; i<argc; i++) {
      obj = (*env)->GetObjectArrayElement(env, argv, i);
      str = JO2C_START(env, obj);
      if (i==0 && (str[0] == '-' || str[0] == MQ_ALFA)) {
	MqBufferLAppendC(largv,context->config.name == NULL ? "java" : context->config.name);
      }
      MqBufferLAppendC(largv,str);
      JO2C_STOP(env,obj,str);
    }
  }

  // create msgque object
  ErrorMqToJavaWithCheck (MqLinkCreateChild (context, parent, &largv));
error:
  return;
}

JNIEXPORT void JNICALL NS(LinkDelete) (
  JNIEnv *	env, 
  jobject	self 
)
{
  MqLinkDelete(CONTEXT);
}

JNIEXPORT void JNICALL NS(LinkConnect) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck (MqLinkConnect(context));
error:
  return;
}

GetC(LinkGetTargetIdent)

