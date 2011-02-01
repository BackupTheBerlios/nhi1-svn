/**
 *  \file       theLink/javamsgque/context_java.h
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef CONTEXT_JAVA__H
#define CONTEXT_JAVA__H

#include <string.h>
#include <stdlib.h>

#include "mqconfig.h"
#include "msgque.h"
#include "debug.h"

// public visible names with package prefix
#define NS(n)	Java_javamsgque_MqS_ ## n

#include "javamsgque_MqS.h"

#define MQ_CONTEXT_S context

// on POSIX I use the -fvisibility=hidden switch
// the default java JNIEXPORT have to be fixed
#if defined(MQ_IS_POSIX)
# undef JNIEXPORT
# define JNIEXPORT __attribute__ ((visibility("default")))
#endif

/*****************************************************************************/
/*                                                                           */
/*                              from misc                                    */
/*                                                                           */
/*****************************************************************************/

#define MqFactorySException() NS(pErrorFromString)(env, __func__, __FILE__, __LINE__, "MqFactoryS exception")
#define ErrorStringToJava(err) NS(pErrorFromString)(env, __func__, __FILE__, __LINE__, err)
#define ErrorMqToJava() NS(pErrorFromMq)(env, context)
#define ErrorMqBufferToJava() NS(pErrorFromMq)(env, buf->context)
#define ErrorMqToJavaWithCheck(PROC) \
  if (MqErrorCheckI(PROC)) { \
    ErrorMqToJava(); \
    goto error; \
  }
#define ErrorMqBufferToJavaWithCheck(PROC) \
  if (MqErrorCheckI(PROC)) { \
    ErrorMqBufferToJava(); \
    goto error; \
  }
#define JavaErrorCheck(PROC) PROC; \
if ((*env)->ExceptionCheck(env) == JNI_TRUE) goto error;
#define JavaErrorCheckNULL(PROC) if ((PROC) == NULL) goto error;

MQ_STR NS(pGetStringFromStaticJavaField)    ( JNIEnv *, jclass, const char*, const char*);
MQ_BOL NS(pGetBoolFromStaticJavaField)	    ( JNIEnv *, jclass, const char*, const char*);
MQ_INT NS(pGetIntFromStaticJavaField)	    ( JNIEnv *, jclass, const char*, const char*);

void		NS(pErrorFromString)		( JNIEnv *, MQ_CST, MQ_CST, const int, MQ_CST);
void		NS(pErrorFromMq)		( JNIEnv *, struct MqS *const);
jmethodID	NS(pGetMethodIdFromObjectField)	( JNIEnv *, jobject, const char *);
const char*	NS(pGetClassName)		( JNIEnv *, jclass );

struct ProcCallS {
  JNIEnv      *env;
  jobject     object;
  jclass      class;
  jmethodID   method;
  jobject     data;
};

enum MqErrorE MQ_DECL NS(ProcCall)	(struct MqS * const, MQ_PTR const);
void          MQ_DECL NS(ProcFree)	(struct MqS const * const, MQ_PTR*);
void	      MQ_DECL NS(ProcCopy)	(struct MqS * const, MQ_PTR*);
MQ_PTR		      NS(ProcCreate)	(JNIEnv*, jobject, jclass, jmethodID, jobject);


/*****************************************************************************/
/*                                                                           */
/*                              context_basic                                */
/*                                                                           */
/*****************************************************************************/

#define XCONTEXT(x)	  ((struct MqS *) (*env)->GetLongField(env,x,NS(FID_MqS_hdl)))
#define CONTEXT		  XCONTEXT(self)
#define SETUP_context	  struct MqS * const context = CONTEXT

#define XFACTORY(x)	  ((struct MqFactoryS *) (*env)->GetLongField(env,x,NS(FID_MqFactoryS_hdl)))
#define FACTORY		  XFACTORY(self)
#define SETUP_factory	  struct MqFactoryS * const factory = FACTORY

#define SET_context(val)  (*env)->SetLongField(env,self,NS(FID_MqS_hdl),(jlong)val)

#define ENV		  ((JNIEnv*) context->threadData)
#define SETUP_env	  JNIEnv *env = ENV

#define SELF		  ((jobject) context->self)
#define SETUP_self	  jobject self = SELF

#define SETUP_buf \
  struct MqBufferS * const buf = ((struct MqBufferS *) (*env)->GetLongField(env,self,NS(FID_MqBufferS_hdl))); \
  if (buf == NULL) { \
    (*env)->ThrowNew(env, NS(Class_NullPointerException), "javamsgque buffer object already deleted");\
    goto error; \
  }

static mq_inline MQ_CST JO2C_START(JNIEnv *env, jstring o) {
  return (o?(*env)->GetStringUTFChars(env,o,NULL):NULL);
}

static mq_inline void JO2C_STOP(JNIEnv *env, jstring o, MQ_CST c) {
  if (c) (*env)->ReleaseStringUTFChars(env,o,c);
}

static mq_inline jstring JC2O(JNIEnv *env, MQ_CST c) {
  return (c?(*env)->NewStringUTF(env,c):NULL);
}

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

static inline void printJC(JNIEnv *env, jstring str) {
  const char * s = JO2C_START(env,str);
  printC(s);
  JO2C_STOP(env,str,s);
}

#endif /* CONTEXT_JAVA__H */


