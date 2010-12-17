/**
 *  \file       theLink/javamsgque/javamsgque_MqFactoryS.h
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include <jni.h>
/* Header for class javamsgque_MqFactoryS */

#ifndef _Included_javamsgque_MqFactoryS
#define _Included_javamsgque_MqFactoryS
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     javamsgque_MqFactoryS
 * Method:    Add
 * Signature: (Ljava/lang/String;Ljava/lang/Class;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqFactoryS_Add
  (JNIEnv *, jclass, jstring, jclass);

/*
 * Class:     javamsgque_MqFactoryS
 * Method:    Call
 * Signature: (Ljava/lang/String;)Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqFactoryS_Call
  (JNIEnv *, jclass, jstring);

/*
 * Class:     javamsgque_MqFactoryS
 * Method:    New
 * Signature: (Ljava/lang/String;Ljava/lang/Class;)Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqFactoryS_New
  (JNIEnv *, jclass, jstring, jclass);

#ifdef __cplusplus
}
#endif
#endif
