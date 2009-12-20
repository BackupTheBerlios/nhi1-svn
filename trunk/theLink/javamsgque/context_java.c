/**
 *  \file       theLink/javamsgque/context_java.c
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

/// global data
JavaVM	    *cached_jvm;

jclass	    NS(Class_MqS), NS(Class_IServerSetup), NS(Class_IServerCleanup),
	    NS(Class_Throwable), NS(Class_Class), NS(Class_Object), NS(Class_IService),
	    NS(Class_NullPointerException), NS(Class_System), NS(Class_RuntimeException),
	    NS(Class_ICallback), NS(Class_StackTraceElement), NS(Class_StringWriter),
	    NS(Class_PrintWriter), NS(Class_MqBufferS), NS(Class_MqSException), 
	    NS(Class_IFactory), NS(Class_IBgError);

jfieldID    NS(FID_MqBufferS_hdl), NS(FID_MqS_hdl), NS(FID_MqSException_num), 
	    NS(FID_MqSException_code), NS(FID_MqSException_txt);

jmethodID   NS(MID_Throwable_getMessage), NS(MID_Class_getName), NS(MID_IService_Service),
	    NS(MID_IServerSetup_ServerSetup), NS(MID_IServerCleanup_ServerCleanup), NS(MID_System_exit),
	    NS(MID_ICallback_Callback), NS(MID_Throwable_printStackTrace),
	    NS(MID_StringWriter_INIT), NS(MID_PrintWriter_INIT), NS(MID_StringWriter_toString),
	    NS(MID_MqSException_INIT), NS(MID_MqBufferS_INIT), NS(MID_MqS_ErrorSet), 
	    NS(MID_IFactory_Factory), NS(MID_IBgError_BgError);

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *jvm, void *reserved)
{
  JNIEnv *env;
  jclass cls;
  cached_jvm = jvm;  /* cache the JavaVM pointer */

  if ((*jvm)->GetEnv(jvm, (void*) &env, JNI_VERSION_1_6)) {
    return JNI_ERR; /* JNI version not supported */
  }

#define check(v,c) if ((v=(c))==NULL) return JNI_ERR
#define checkC(v,n) check(cls, (*env)->FindClass(env, n));\
    check(v, (*env)->NewGlobalRef(env, cls));
#define checkF(v,c,n,s) check(v, (*env)->GetFieldID(env,c,n,s));
#define checkSF(v,c,n,s) check(v, (*env)->GetStaticFieldID(env,c,n,s));
#define checkM(v,c,n,s) check(v, (*env)->GetMethodID(env,c,n,s));
#define checkSM(v,c,n,s) check(v, (*env)->GetStaticMethodID(env,c,n,s));

  // setup "jclass"
  checkC(NS(Class_MqS),			    "javamsgque/MqS");
  checkC(NS(Class_MqBufferS),		    "javamsgque/MqBufferS");
  checkC(NS(Class_MqSException),	    "javamsgque/MqSException");
  checkC(NS(Class_IService),		    "javamsgque/IService");
  checkC(NS(Class_IServerSetup),	    "javamsgque/IServerSetup");
  checkC(NS(Class_IServerCleanup),	    "javamsgque/IServerCleanup");
  checkC(NS(Class_ICallback),		    "javamsgque/ICallback");
  checkC(NS(Class_IFactory),		    "javamsgque/IFactory");
  checkC(NS(Class_IBgError),		    "javamsgque/IBgError");
  checkC(NS(Class_RuntimeException),	    "java/lang/RuntimeException");
  checkC(NS(Class_NullPointerException),    "java/lang/NullPointerException");
  checkC(NS(Class_Throwable),		    "java/lang/Throwable");
  checkC(NS(Class_Class),		    "java/lang/Class");
  checkC(NS(Class_Object),		    "java/lang/Object");
  checkC(NS(Class_System),		    "java/lang/System");
  checkC(NS(Class_StringWriter),	    "java/io/StringWriter");
  checkC(NS(Class_PrintWriter),		    "java/io/PrintWriter");

  // setup "jfieldID"
  checkF(NS(FID_MqS_hdl),		    NS(Class_MqS),	    "hdl",		"J");
  checkF(NS(FID_MqBufferS_hdl),		    NS(Class_MqBufferS),    "hdl",		"J");
  checkF(NS(FID_MqSException_num),	    NS(Class_MqSException), "p_num",		"I");
  checkF(NS(FID_MqSException_code),	    NS(Class_MqSException), "p_code",		"I");
  checkF(NS(FID_MqSException_txt),	    NS(Class_MqSException), "p_txt",		"Ljava/lang/String;");

  // setup "jmethodID"
  checkM(NS(MID_Throwable_getMessage),		NS(Class_Throwable),	  "getMessage",		"()Ljava/lang/String;");
  checkM(NS(MID_Throwable_printStackTrace),	NS(Class_Throwable),	  "printStackTrace",	"(Ljava/io/PrintWriter;)V");
  checkM(NS(MID_Class_getName),			NS(Class_Class),	  "getName",		"()Ljava/lang/String;");
  checkM(NS(MID_IService_Service),		NS(Class_IService),	  "Service",		"(Ljavamsgque/MqS;)V");
  checkM(NS(MID_IServerSetup_ServerSetup),	NS(Class_IServerSetup),	  "ServerSetup",	"()V");
  checkM(NS(MID_IServerCleanup_ServerCleanup),	NS(Class_IServerCleanup), "ServerCleanup",	"()V");
  checkM(NS(MID_StringWriter_INIT),		NS(Class_StringWriter),	  "<init>",		"()V");
  checkM(NS(MID_StringWriter_toString),		NS(Class_StringWriter),	  "toString",		"()Ljava/lang/String;");
  checkM(NS(MID_PrintWriter_INIT),		NS(Class_PrintWriter),	  "<init>",		"(Ljava/io/Writer;)V");
  checkSM(NS(MID_System_exit),			NS(Class_System),	  "exit",		"(I)V");
  checkM(NS(MID_ICallback_Callback),		NS(Class_ICallback),	  "Callback",		"(Ljavamsgque/MqS;)V");
  checkM(NS(MID_IFactory_Factory),		NS(Class_IFactory),	  "Factory",		"()Ljavamsgque/MqS;");
  checkM(NS(MID_MqSException_INIT),		NS(Class_MqSException),	  "<init>",		"(IILjava/lang/String;)V");
  checkM(NS(MID_MqBufferS_INIT),		NS(Class_MqBufferS),	  "<init>",		"(J)V");
  checkM(NS(MID_MqS_ErrorSet),			NS(Class_MqS),		  "ErrorSet",		"(Ljava/lang/Throwable;)V");
  checkM(NS(MID_IBgError_BgError),		NS(Class_IBgError),	  "BgError",		"()V");

#undef check
#undef checkC
#undef checkF
#undef checkSF
#undef checkM

  return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL 
JNI_OnUnload(JavaVM *jvm, void *reserved)
{
  JNIEnv *env;
  void *val;
  if ((*jvm)->GetEnv(jvm, &val, JNI_VERSION_1_6)) {
    return;
  }
  env = (JNIEnv*) val;
  (*env)->DeleteGlobalRef(env, NS(Class_MqS));
  (*env)->DeleteGlobalRef(env, NS(Class_MqBufferS));
  (*env)->DeleteGlobalRef(env, NS(Class_MqSException));
  (*env)->DeleteGlobalRef(env, NS(Class_IService));
  (*env)->DeleteGlobalRef(env, NS(Class_IServerSetup));
  (*env)->DeleteGlobalRef(env, NS(Class_IServerCleanup));
  (*env)->DeleteGlobalRef(env, NS(Class_ICallback));
  (*env)->DeleteGlobalRef(env, NS(Class_IFactory));
  (*env)->DeleteGlobalRef(env, NS(Class_IBgError));
  (*env)->DeleteGlobalRef(env, NS(Class_Throwable));
  (*env)->DeleteGlobalRef(env, NS(Class_Class));
  (*env)->DeleteGlobalRef(env, NS(Class_Object));
  (*env)->DeleteGlobalRef(env, NS(Class_System));
  (*env)->DeleteGlobalRef(env, NS(Class_StringWriter));
  (*env)->DeleteGlobalRef(env, NS(Class_PrintWriter));
  (*env)->DeleteGlobalRef(env, NS(Class_RuntimeException));
  (*env)->DeleteGlobalRef(env, NS(Class_NullPointerException));
  return;
}

static void MQ_DECL NS(FactoryDelete) (
  struct MqS * context,
  MQ_BOL dofactoryCleanup,
  MQ_PTR data
)
{
  SETUP_env;
  SETUP_self; 
  NS(ContextDelete) (env, self);
}

static enum MqErrorE MQ_DECL NS(FactoryCreate) (
  struct MqS * const tmpl,
  enum MqFactoryE create,
  MQ_PTR data,
  struct MqS  ** contextP
)
{
  JNIEnv * env = NULL;

  // get env
  if (create == MQ_FACTORY_NEW_THREAD) {
    JavaVMAttachArgs args = {JNI_VERSION_1_6, "libmsgque", NULL};
    if ((*cached_jvm)->AttachCurrentThread(cached_jvm, (void**)&env, (void**)&args) != JNI_OK) {
      return MqErrorC (tmpl, __func__, -1, "unable to 'AttachCurrentThread'");
    }
  } else {
    env = ((JNIEnv*) tmpl->threadData);
  }

  // create new object
  *contextP = XCONTEXT((*env)->CallObjectMethod(env, (jobject)tmpl->self, NS(MID_IFactory_Factory)));
  if((*env)->ExceptionCheck(env) == JNI_TRUE) {
    (*env)->CallVoidMethod(env, tmpl->self, NS(MID_MqS_ErrorSet), (*env)->ExceptionOccurred(env));
    (*env)->ExceptionClear(env);
    return MqErrorStack(tmpl);
  }

  MqConfigDup (*contextP, tmpl);
  return MQ_OK;
}

static void 
MQ_DECL NS(ThreadExit) (
  int exitnum
)
{
  (*cached_jvm)->DetachCurrentThread(cached_jvm);
}

static void
MQ_DECL NS(ProcessExit) (
  int exitnum
)
{
  JNIEnv *env;
  if ((*cached_jvm)->GetEnv(cached_jvm, (void*) &env, JNI_VERSION_1_6) == JNI_OK) {
    (*env)->CallStaticVoidMethod(env, NS(Class_System), NS(MID_System_exit), (jint) exitnum);
  }
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
    MqBufferLAppendC(largv,context->config.name == NULL ? "java" : context->config.name);
    for (i=0; i<argc; i++) {
      obj = (*env)->GetObjectArrayElement(env, argv, i);
      str = JO2C_START(env, obj);
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
    MqBufferLAppendC(largv,context->config.name == NULL ? "java" : context->config.name);
    for (i=0; i<argc; i++) {
      obj = (*env)->GetObjectArrayElement(env, argv, i);
      str = JO2C_START(env, obj);
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

JNIEXPORT void JNICALL NS(Exit) (
  JNIEnv *	env, 
  jobject	self 
)
{
  MqExit(CONTEXT);
}

JNIEXPORT void JNICALL NS(pProcessEvent) (
  JNIEnv *	env, 
  jobject	self, 
  jint		timeout,
  jint		flags
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqProcessEvent(context, timeout, (enum MqWaitOnEventE) flags));
error:
  return;
}

JNIEXPORT void JNICALL NS(Init) (
  JNIEnv *	env, 
  jclass	cls,
  jobjectArray	argv
)
{
  jobject obj;
  const char *str; 
  jsize argc = (*env)->GetArrayLength(env, argv);
  struct MqBufferLS *initB = MqInitCreate();
  jsize i;
  for (i=0; i<argc; i++) {
    obj = (*env)->GetObjectArrayElement(env, argv, i);
    str = JO2C_START(env, obj);
    MqBufferLAppendC(initB,str);
    JO2C_STOP(env,obj,str);
  }
}

JNIEXPORT void JNICALL NS(ContextCreate) (
  JNIEnv  *env, 
  jobject self
)
{
  struct MqS *context = MqContextCreate (0, NULL);
  MQ_PTR call;

  context->self = (MQ_PTR) (*env)->NewGlobalRef(env, self);
  if (context->self == NULL) {
    ErrorStringToJava("unable to create global reference for 'context->class'");
    goto error;
  }

  context->threadData		 = (MQ_PTR) env;
  MqConfigSetIgnoreFork (context, MQ_YES);
  context->setup.Parent.fCreate  = MqDefaultLinkCreate;
  context->setup.Child.fCreate   = MqDefaultLinkCreate;
  context->setup.fProcessExit    = NS(ProcessExit);
  context->setup.fThreadExit	 = NS(ThreadExit);

  // check for Server
  if ((*env)->IsInstanceOf(env, self, NS(Class_IServerSetup)) == JNI_TRUE) {
    ErrorMqToJavaWithCheck (NS(ProcCreate)(context, self, NULL, NS(MID_IServerSetup_ServerSetup), NULL, &call));
    MqConfigSetServerSetup (context, NS(ProcCall), call, NS(ProcFree), NS(ProcCopy));
  }
  if ((*env)->IsInstanceOf(env, self, NS(Class_IServerCleanup)) == JNI_TRUE) {
    ErrorMqToJavaWithCheck (NS(ProcCreate)(context, self, NULL, NS(MID_IServerCleanup_ServerCleanup), NULL, &call));
    MqConfigSetServerCleanup (context, NS(ProcCall), call, NS(ProcFree), NS(ProcCopy));
  }

  // check for Factory
  if ((*env)->IsInstanceOf(env, self, NS(Class_IFactory)) == JNI_TRUE) {
    context->setup.Factory.Create.fCall = NS(FactoryCreate);
  }
  context->setup.Factory.Delete.fCall = NS(FactoryDelete);

  // check for BgError
  if ((*env)->IsInstanceOf(env, self, NS(Class_IBgError)) == JNI_TRUE) {
    ErrorMqToJavaWithCheck (NS(ProcCreate)(context, self, NULL, NS(MID_IBgError_BgError), NULL, &call));
    MqConfigSetBgError (context, NS(ProcCall), call, NS(ProcFree), NS(ProcCopy));
  }

  SET_context(context);
  return;

error:
  MqContextDelete(&context);
  SET_context(0L);
  return;
}

JNIEXPORT void JNICALL NS(ContextDelete) (
  JNIEnv  *env, 
  jobject self
)
{
  struct MqS * context = CONTEXT;
  if (context == NULL) {
    return;
  } else {
    SETUP_self;
    SET_context(0L);
    MqContextDelete (&context);
    if (self != NULL) (*env)->DeleteGlobalRef(env, self);
  }
}



