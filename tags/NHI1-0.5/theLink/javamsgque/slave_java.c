/**
 *  \file       theLink/javamsgque/slave_java.c
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

extern jfieldID NS(FID_MqS_hdl);
extern jclass NS(Class_NullPointerException);

enum MqErrorE MQ_DECL NS(ParentCreate) (struct MqConfigS*, struct MqBufferLS*, struct MqBufferLS*, struct MqS**);

JNIEXPORT void JNICALL NS(SlaveWorker) (
  JNIEnv *	env, 
  jobject	self, 
  jint		id, 
  jobjectArray	args
)
{
  SETUP_context;
  struct MqBufferLS * largs = NULL;

  // fill the args
  if (args != NULL) {
    jobject obj;
    const char *str;
    int i;
    jsize argc;
    argc = (*env)->GetArrayLength(env, args);
    largs = MqBufferLCreate (argc);
    for (i=0; i<argc; i++) {
      obj = (*env)->GetObjectArrayElement(env, args, i);
      str = JO2C_START(env, obj);
      MqBufferLAppendC(largs,str);
      JO2C_STOP(env,obj,str);
    }
  }

  // create Worker
  ErrorMqToJavaWithCheck(MqSlaveWorker(context, id, &largs));
error:
  return;
}

JNIEXPORT void JNICALL NS(SlaveCreate) (
  JNIEnv *  env, 
  jobject   self, 
  jint	    id,
  jobject   slave
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSlaveCreate(context, id, XCONTEXT(slave)));
error:
  return;
}

JNIEXPORT void JNICALL NS(SlaveDelete) (
  JNIEnv *  env, 
  jobject   self, 
  jint	    id
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSlaveDelete (context, id));
error:
  return;
}

JNIEXPORT jobject JNICALL NS(SlaveGet) (
  JNIEnv *  env, 
  jobject   self, 
  jint	    id
)
{
  struct MqS * const slave = MqSlaveGet(CONTEXT, id);
  return (slave == NULL ? NULL : (jobject)slave->self);
}

JNIEXPORT jobject JNICALL NS(SlaveGetMaster) (
  JNIEnv    *env, 
  jobject   self
)
{
  struct MqS * const master = MqSlaveGetMaster(CONTEXT);
  return (master == NULL ? NULL : ((jobject)master->self));
}

GetO(SlaveIs)

