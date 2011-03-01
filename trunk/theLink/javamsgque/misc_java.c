/**
 *  \file       theLink/javamsgque/misc_java.c
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

extern jmethodID    NS(MID_Throwable_getMessage);
extern jfieldID	    NS(MID_MqSException_num);
extern jfieldID	    NS(MID_MqSException_code);
extern jfieldID	    NS(MID_MqSException_txt);
extern jmethodID    NS(MID_MqS_ErrorSet);
extern jmethodID    NS(MID_MqSException_INIT);
extern jmethodID    NS(MID_Class_getName);
extern jmethodID    NS(MID_StringWriter_INIT);
extern jmethodID    NS(MID_PrintWriter_INIT);
extern jmethodID    NS(MID_StringWriter_toString);
extern jmethodID    NS(MID_Throwable_printStackTrace);
extern jclass	    NS(Class_MqSException);
extern jclass	    NS(Class_StringWriter);
extern jclass	    NS(Class_PrintWriter);

MQ_PTR
NS(ProcCreate) (
  JNIEnv*   env,
  jobject   object,
  jclass    class,
  jmethodID method,
  jobject   data
)
{
  struct ProcCallS * call = MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(*call));
  if (object != NULL) {
    JavaErrorCheckNULL (call->object = (*env)->NewGlobalRef(env, object));
  }
  if (class != NULL) {
    JavaErrorCheckNULL (call->class = (*env)->NewGlobalRef(env, class));
  }
  if (data != NULL) {
    JavaErrorCheckNULL (call->data = (*env)->NewGlobalRef(env, data));
  }
  call->env = env;
  call->method = method;
  return call;
error:
  if (call->object) (*env)->DeleteGlobalRef(env, call->object);
  if (call->class)  (*env)->DeleteGlobalRef(env, call->class);
  if (call->data )  (*env)->DeleteGlobalRef(env, call->data);
  MqSysFree(call);
  return NULL;
}

void MQ_DECL
NS(ProcCopy) (
  struct MqS * const context,
  MQ_PTR * const dataP
)
{
  struct ProcCallS * old = (struct ProcCallS *) *dataP;
  JavaErrorCheckNULL(*dataP = NS(ProcCreate) (old->env, old->object, old->class, old->method, old->data));
  return;
error:
  MqPanicC(context, __func__, -1, "unable to copy proc data");
}

void MQ_DECL 
NS(ProcFree) (
  struct MqS const * const context, 
  MQ_PTR *dataP
)
{
  struct ProcCallS * call = (struct ProcCallS *) *dataP;
  JNIEnv *env = call->env;
  if (call->object != NULL) (*env)->DeleteGlobalRef(env, call->object);
  if (call->class  != NULL) (*env)->DeleteGlobalRef(env, call->class );
  if (call->data   != NULL) (*env)->DeleteGlobalRef(env, call->data  );
  MqSysFree(*dataP);
}

enum MqErrorE MQ_DECL
NS(ProcCall) (
  struct MqS * const context,
  MQ_PTR const callP
)
{
  struct ProcCallS const * const call = (struct ProcCallS const * const) callP;
  JNIEnv *env = call->env;

  // clean JAVA and libmsgque error
  //(*env)->ExceptionClear(env);
  //MqErrorReset(context);

  // call the function
  if (call->class == NULL) {
    // method call of the same class
    if (call->data == NULL) {
      (*env)->CallVoidMethod(env, call->object, call->method);
    } else {
      (*env)->CallVoidMethod(env, call->object, call->method, call->data);
    }
  } else if (call->object == NULL) {
    // static method call returning an object
    if (call->data == NULL) {
      (*env)->CallStaticObjectMethod(env, call->class, call->method);
    } else {
      (*env)->CallStaticObjectMethod(env, call->class, call->method, call->data);
    }
  } else {
    // method call of an other class
    if (call->data == NULL) {
      (*env)->CallNonvirtualVoidMethod(env, call->object, call->class, call->method);
    } else {
      (*env)->CallNonvirtualVoidMethod(env, call->object, call->class, call->method, call->data);
    }
  }

  // check on error
  if((*env)->ExceptionCheck(env) != JNI_FALSE) {
    // is the EROOR from "java" or "javamsgque"
    NS(ErrorSet)(env, context->self, (*env)->ExceptionOccurred(env));
  }
  return MqErrorGetCodeI(context);
}

// create a JAVA error from a string
void NS(pErrorFromString) (
  JNIEnv *  env,
  MQ_CST    func,
  MQ_CST    file,
  const int line,
  MQ_CST    err
)
{
  const int len = (strlen(err)+200);
  char * buf = MqSysMalloc(MQ_ERROR_PANIC, len);
  char * fn = MqSysBasename(file, MQ_YES);
  mq_snprintf(buf,len,"\n        at %s(%s:%i)\n\n%s\n", func, fn , line, err);
  (*env)->Throw(env, (jthrowable) (*env)->NewObject(env, NS(Class_MqSException), 
      NS(MID_MqSException_INIT), (jint) 1, (jint) MQ_ERROR, JC2O(env, buf))
  );
  MqSysFree(fn);
  MqSysFree(buf);
}

// convert error from "javamsgque" error object into an instance of the "MqSException" class
void NS(pErrorFromMq) (
  JNIEnv *	      env,
  struct MqS *const   error
)
{
  jthrowable ex = (jthrowable) (*env)->NewObject(env, NS(Class_MqSException), 
    NS(MID_MqSException_INIT), (jint) MqErrorGetNumI(error), (jint) MqErrorGetCodeI(error), 
      JC2O(env, MqErrorGetText(error)));
  MqErrorReset(error);
  (*env)->Throw(env, ex);
}

/*
jmethodID
NS(pGetMethodIdFromObjectField)(
  JNIEnv *      env,
  jobject       obj,
  const char *  name
)
{
  const char *str;
  const jclass class = (*env)->GetObjectClass(env, obj);
  const jfieldID id = (*env)->GetFieldID(env, class, name, "Ljava/lang/String;");
  const jobject fieldO = (*env)->GetObjectField(env, obj, id);
  jmethodID method;
  if (fieldO == NULL) return NULL;
  str = JO2C_START(env,fieldO,NULL);
  method = (*env)->GetMethodID(env, class, str, "()V");
  JO2C_STOP(env,fieldO,str);
  return method;
}
*/

const char*
NS(pGetClassName)(
  JNIEnv *	env,
  jclass	class
)
{
  const char *msg, *ret;

  // call the "getName" on "class"
  jstring getNameO = (*env)->CallObjectMethod(env, class, NS(MID_Class_getName));
  if (getNameO == NULL) return MqSysStrDup(MQ_ERROR_PRINT, "");
  
  // get the "string" out of "getNameO"
  msg = JO2C_START(env, getNameO);
  ret = MqSysStrDup(MQ_ERROR_PRINT, msg);
  JO2C_STOP(env, getNameO, msg);

  return ret;
}

