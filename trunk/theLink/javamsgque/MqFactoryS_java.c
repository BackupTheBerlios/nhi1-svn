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

extern JavaVM *cached_jvm;
extern jfieldID	    NS(FID_MqS_hdl);

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
  struct FactoryCallS * call = (struct FactoryCallS *) *dataP;
  JNIEnv *env = call->env;
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
  JavaErrorCheckNULL(call = NS(FactorySetup)(env, cbClass, callback));
  str = JO2C_START(env,ident);
  ret = MqFactoryAdd(str, NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL);
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
  JavaErrorCheckNULL(call = NS(FactorySetup)(env, cbClass, callback));
  str = JO2C_START(env,ident);
  ret = MqFactoryDefault(str, NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), NS(FactoryDelete), NULL, NULL, NULL);
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
  JavaErrorCheckNULL(call = NS(FactorySetup)(env, cbClass, callback));
  str = JO2C_START(env,ident);
  ret = MqFactoryNew (str, NS(FactoryCreate), call, NS(FactoryFree), NS(FactoryCopy), 
	  NS(FactoryDelete), NULL, NULL, NULL, NULL, &mqctx);
  JO2C_STOP(env,ident,str);
  MqFactoryErrorCheck(ret);
  return mqctx->self;
error:
  ErrorStringToJava(MqFactoryErrorMsg(ret));
  return NULL;  
}
