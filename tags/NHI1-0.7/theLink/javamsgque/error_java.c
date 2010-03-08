/**
 *  \file       theLink/javamsgque/error_java.c
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

extern jfieldID   NS(FID_MqS_hdl);
extern jfieldID   NS(FID_MqSException_txt);
extern jfieldID   NS(FID_MqSException_num);
extern jfieldID   NS(FID_MqSException_code);
extern jmethodID  NS(MID_StringWriter_INIT);
extern jmethodID  NS(MID_PrintWriter_INIT);
extern jmethodID  NS(MID_Throwable_printStackTrace);
extern jmethodID  NS(MID_StringWriter_toString);
extern jclass	  NS(Class_MqSException);
extern jclass	  NS(Class_StringWriter);
extern jclass	  NS(Class_PrintWriter);

JNIEXPORT void JNICALL NS(ErrorC) (
  JNIEnv    *env,
  jobject   self,
  jstring   func,
  jint      num,
  jstring   message
)
{
  SETUP_context;
  const char *funcC;
  const char *mesgC;
  funcC = JO2C_START(env, func);
  mesgC = JO2C_START(env, message);
  MqErrorC(context, funcC, num, mesgC);
  JO2C_STOP(env, func, funcC);
  JO2C_STOP(env, message, mesgC);
}

JNIEXPORT void JNICALL NS(ErrorRaise) (
  JNIEnv    *env,
  jobject   self
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqErrorGetCodeI(context));
error:
  return;
}

JNIEXPORT void JNICALL NS(ErrorReset) (
  JNIEnv    *env,
  jobject   self
)
{
  SETUP_context;
  MqErrorReset(context);
}

JNIEXPORT void JNICALL NS(ErrorSetCONTINUE) (
  JNIEnv    *env,
  jobject   self
)
{
  MqErrorSetCONTINUE(CONTEXT);
}

JNIEXPORT void JNICALL NS(ErrorSet) (
  JNIEnv      *env,
  jobject     self,
  jthrowable  ex
)
{
  SETUP_context;

  // is the EROOR from "java" or "javamsgque"
  if ((*env)->IsInstanceOf(env, ex, NS(Class_MqSException))) {
  
    // error is from "javamsgque"
    const char *txtC = JO2C_START(
      env, (jstring) (*env)->GetObjectField(env,ex,NS(FID_MqSException_txt))
    );
    MqErrorSet(context,
      (MQ_INT) (*env)->GetIntField(env,ex,NS(FID_MqSException_num)), 
      (enum MqErrorE) (*env)->GetIntField(env,ex,NS(FID_MqSException_code)), 
      txtC,
      NULL
    );
    JO2C_STOP(env, ex, txtC);

  } else {
    // error is from "java"
    jstring message = NULL;

    // 1. if no "message" is available -> get StackTrace
    // 
    // final Writer result = new StringWriter();
    // final PrintWriter printWriter = new PrintWriter(result);
    // ex.printStackTrace(printWriter);
    // result.toString();
    // 
    jobject stringWriter = (*env)->NewObject(env, NS(Class_StringWriter), NS(MID_StringWriter_INIT));
    jobject printWriter = (*env)->NewObject(env, NS(Class_PrintWriter), NS(MID_PrintWriter_INIT), stringWriter);
    (*env)->CallVoidMethod(env, ex, NS(MID_Throwable_printStackTrace), printWriter);
    message = (jstring) (*env)->CallObjectMethod(env, stringWriter, NS(MID_StringWriter_toString));

    // 2. Add java-error-message to MQ error
    if (message != NULL) {
      MQ_CST str = JO2C_START(env,message);
      MqErrorC(context, __func__, -1, str);
      JO2C_STOP(env,message,str);
    } else {
      MqErrorC(context, __func__, -1, "unknown Exception");
    }
  }
  (*env)->ExceptionClear(env);
}

JNIEXPORT jint JNICALL NS(ErrorGetNum) (
  JNIEnv      *env,
  jobject     self
)
{
  return MqErrorGetNumI(CONTEXT);
}

JNIEXPORT jstring JNICALL NS(ErrorGetText) (
  JNIEnv    *env, 
  jobject   self
)
{
  return JC2O(env,MqErrorGetText(CONTEXT));
}

JNIEXPORT void JNICALL NS(ErrorPrint) (
  JNIEnv      *env,
  jobject     self
)
{
  MqErrorPrint (CONTEXT);
}

JNIEXPORT void JNICALL NS(SendERROR) (
  JNIEnv *	env, 
  jobject	self 
)
{
  SETUP_context;
  ErrorMqToJavaWithCheck(MqSendERROR(context));
error:
  return;
}

GetO(ErrorIsEXIT)
