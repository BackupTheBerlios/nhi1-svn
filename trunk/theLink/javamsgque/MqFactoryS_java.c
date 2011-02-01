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

#define NF(n)	    Java_javamsgque_MqFactoryS_ ## n

extern JavaVM	    *cached_jvm;
extern jclass	    NS(Class_MqS);
extern jfieldID	    NS(FID_MqS_hdl);
extern jfieldID	    NS(FID_MqFactoryS_hdl);
extern jclass       NS(Class_MqFactoryS);
extern jmethodID    NS(MID_MqFactoryS_INIT);
extern jmethodID    NS(MID_MqS_INIT);

struct FactoryCallS {
  JNIEnv      *env;
  jclass      class;
  jmethodID   method;
};

static MQ_PTR NS(FactorySetup) (
  JNIEnv*   env,
  jclass    class,
  jmethodID method
)
{
  struct FactoryCallS * call = MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(*call));
  JavaErrorCheckNULL (call->class = (*env)->NewGlobalRef(env, class));
  call->env = env;
  call->method = method;
  return call;
error:
  MqSysFree(call);
  return NULL;
}

static void
NS(FactoryCopy) (
  MQ_PTR *dataP
)
{
  struct FactoryCallS * call = (struct FactoryCallS *) *dataP;
  *dataP = NS(FactorySetup) (call->env, call->class, call->method);
  return;
}

static void MQ_DECL 
NS(FactoryFree) (
  MQ_PTR *dataP
)
{
  //struct FactoryCallS * call = (struct FactoryCallS *) *dataP;
  //JNIEnv *env = call->env;
  // this function will be called on !exit! but "java" is already gone -> no java cleanup
  //(*env)->DeleteGlobalRef(env, call->class);
  MqSysFree(*dataP);
}

static void MQ_DECL NS(FactoryDelete) (
  struct MqS * context,
  MQ_BOL dofactoryCleanup,
  struct MqFactoryS* const item
)
{
  SETUP_env;
  SETUP_self; 
  NS(ContextDelete) (env, self);
}

static enum MqErrorE MQ_DECL NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS* const item,
  struct MqS  ** contextP
)
{
  jobject tmplO = create == MQ_FACTORY_NEW_INIT ? NULL : tmpl->self;
  struct MqS * mqctx;
  JNIEnv * env = NULL;
  struct FactoryCallS const * const call = (struct FactoryCallS const * const) item->Create.data;

  // get env
  if (create == MQ_FACTORY_NEW_THREAD) {
    JavaVMAttachArgs args = {JNI_VERSION_1_6, "libmsgque", NULL};
    if ((*cached_jvm)->AttachCurrentThread(cached_jvm, (void**)&env, (void**)&args) != JNI_OK) {
      return MqErrorC (tmpl, __func__, -1, "unable to 'AttachCurrentThread'");
    }
  } else if (create == MQ_FACTORY_NEW_INIT) {
    env = ((JNIEnv*) call->env);
  } else {
    env = ((JNIEnv*) tmpl->threadData);
  }

  // create new object
  {
    jobject obj = (*env)->NewObject(env, call->class, call->method, tmplO);
    if (obj == NULL) goto error1;
    mqctx = XCONTEXT(obj);
    if((*env)->ExceptionCheck(env) == JNI_TRUE) goto error;
  }

  // copy setup data and initialize "setup" data
  if (create != MQ_FACTORY_NEW_INIT) {
    MqSetupDup(mqctx, tmpl);
  }

  *contextP = mqctx;
  return MQ_OK;

error:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    MqErrorCopy (tmpl, mqctx);
    MqContextDelete (&mqctx);
    return MqErrorStack(tmpl);
  } else {
    return MQ_ERROR;
  }

error1:
  *contextP = NULL;
  if (create != MQ_FACTORY_NEW_INIT) {
    NS(ErrorSet)(env, tmpl->self, (*env)->ExceptionOccurred(env));
    return MqErrorGetCode(tmpl);
  } else {
    return MQ_ERROR;
  }
}

JNIEXPORT jobject JNICALL NF(Add) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  jobject self;
  struct MqFactoryS * factory;
  MQ_PTR call;
  jmethodID callback;
  const char * str;
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(FactorySetup)(env, cbClass, callback));
  str = JO2C_START(env,ident);
  factory = MqFactoryAdd(str, NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL);
  JO2C_STOP(env,ident,str);
  self = (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) factory);
  return self;
error:
  MqFactorySException();
  return NULL;  
}

JNIEXPORT jobject JNICALL NF(Default) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident,
  jclass  cbClass
)
{
  struct MqFactoryS * factory;
  MQ_PTR call;
  jmethodID callback;
  const char * str;
  JavaErrorCheckNULL(callback = (*env)->GetMethodID(env,cbClass,"<init>","(Ljavamsgque/MqS;)V"));
  JavaErrorCheckNULL(call = NS(FactorySetup)(env, cbClass, callback));
  str = JO2C_START(env,ident);
  factory = MqFactoryDefault(str, NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL);
  JO2C_STOP(env,ident,str);
  return (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) factory);
error:
  MqFactorySException();
  return NULL;  
}

JNIEXPORT jobject JNICALL NF(Get__Ljava_lang_String_2) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident
)
{
  struct MqFactoryS * factory;
  const char * str;
  str = JO2C_START(env,ident);
  factory = MqFactoryGet(str);
  JO2C_STOP(env,ident,str);
  return (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) factory);
}

JNIEXPORT jobject JNICALL NF(Get__) (
  JNIEnv  *env, 
  jclass  class
)
{
  return (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) MqFactoryGet(NULL));
}

JNIEXPORT jobject JNICALL NF(GetCalled) (
  JNIEnv  *env, 
  jclass  class,
  jstring ident
)
{
  struct MqFactoryS * factory;
  const char * str;
  str = JO2C_START(env,ident);
  factory = MqFactoryGetCalled(str);
  JO2C_STOP(env,ident,str);
  return (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) factory);
}

JNIEXPORT jstring JNICALL NF(DefaultIdent) (
  JNIEnv  *env, 
  jclass  class
)
{
  MQ_CST str = MqFactoryDefaultIdent();
  return JC2O(env,str);
}

JNIEXPORT jobject JNICALL NF(New) (
  JNIEnv  *env, 
  jobject self
)
{
  SETUP_factory;
  struct MqS * mqctx;
  MqErrorCheck (MqFactoryNew (factory, (MQ_PTR)env, &mqctx));
  return mqctx->self;
error:
  MqFactorySException();
  return NULL;  
}

JNIEXPORT jobject JNICALL NF(Copy) (
  JNIEnv  *env, 
  jobject self,
  jstring ident
)
{
  const char * str;
  SETUP_factory;
  struct MqFactoryS * ret;
  str = JO2C_START(env,ident);
  ret = MqFactoryCopy (factory, str);
  JO2C_STOP(env,ident,str);
  return (*env)->NewObject(env, NS(Class_MqFactoryS), NS(MID_MqFactoryS_INIT), (jlong) ret);
}

void FactoryInit(JNIEnv *env) {
  MQ_PTR call;
  if (strcmp(MqFactoryDefaultIdent(),"libmsgque")) return;
  call = NS(FactorySetup)(env, NS(Class_MqS), NS(MID_MqS_INIT));
  if (call != NULL) {
    MqFactoryDefault("javamsgque", NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL);
  } else {
    MqFactorySException();
  }
}

