/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class javamsgque_MqS */

#ifndef _Included_javamsgque_MqS
#define _Included_javamsgque_MqS
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     javamsgque_MqS
 * Method:    Init
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_Init
  (JNIEnv *, jclass, jobjectArray);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetName
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetSrvName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetSrvName
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIdent
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIdent
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetBuffersize
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetBuffersize
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetDebug
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetDebug
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetTimeout
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetTimeout
  (JNIEnv *, jobject, jlong);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIsSilent
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIsSilent
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIsServer
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIsServer
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIsString
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIsString
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIgnoreExit
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIgnoreExit
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     javamsgque_MqS
 * Method:    pConfigSetStartAs
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_pConfigSetStartAs
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIoTcp
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIoTcp
  (JNIEnv *, jobject, jstring, jstring, jstring, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIoUdsFile
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIoUdsFile
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigSetIoPipeSocket
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ConfigSetIoPipeSocket
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIsServer
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ConfigGetIsServer
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIsString
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ConfigGetIsString
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIsSilent
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ConfigGetIsSilent
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetBuffersize
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ConfigGetBuffersize
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetTimeout
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_javamsgque_MqS_ConfigGetTimeout
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetName
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetSrvName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetSrvName
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIdent
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIdent
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetDebug
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ConfigGetDebug
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    pConfigGetStartAs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_pConfigGetStartAs
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoTcpHost
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIoTcpHost
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoTcpPort
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIoTcpPort
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoTcpMyHost
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIoTcpMyHost
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoTcpMyPort
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIoTcpMyPort
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoUdsFile
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ConfigGetIoUdsFile
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ConfigGetIoPipeSocket
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ConfigGetIoPipeSocket
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ContextCreate
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ContextCreate
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ContextDelete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ContextDelete
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    Exit
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_Exit
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LogC
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_LogC
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkCreate
 * Signature: ([Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_LinkCreate
  (JNIEnv *, jobject, jobjectArray);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkCreateChild
 * Signature: (Ljavamsgque/MqS;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_LinkCreateChild
  (JNIEnv *, jobject, jobject, jobjectArray);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkDelete
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_LinkDelete
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkConnect
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_LinkConnect
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkGetParent
 * Signature: ()Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_LinkGetParent
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkGetTargetIdent
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_LinkGetTargetIdent
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkIsParent
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_LinkIsParent
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkIsConnected
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_LinkIsConnected
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    LinkGetCtxId
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_LinkGetCtxId
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceGetToken
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ServiceGetToken
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceGetFilter
 * Signature: ()Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_ServiceGetFilter__
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceGetFilter
 * Signature: (I)Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_ServiceGetFilter__I
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceIsTransaction
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ServiceIsTransaction
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceCreate
 * Signature: (Ljava/lang/String;Ljavamsgque/IService;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ServiceCreate
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceProxy
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ServiceProxy__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceProxy
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ServiceProxy__Ljava_lang_String_2I
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ServiceDelete
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ServiceDelete
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    pProcessEvent
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_pProcessEvent
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorC
 * Signature: (Ljava/lang/String;ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorC
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorRaise
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorRaise
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorReset
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorReset
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorSet
 * Signature: (Ljava/lang/Throwable;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorSet
  (JNIEnv *, jobject, jthrowable);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorSetCONTINUE
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorSetCONTINUE
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorSetEXIT
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorSetEXIT
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorIsEXIT
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ErrorIsEXIT
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorGetNum
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ErrorGetNum
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorGetText
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ErrorGetText
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ErrorPrint
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ErrorPrint
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadY
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_javamsgque_MqS_ReadY
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadO
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ReadO
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadS
 * Signature: ()S
 */
JNIEXPORT jshort JNICALL Java_javamsgque_MqS_ReadS
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadI
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ReadI
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadF
 * Signature: ()F
 */
JNIEXPORT jfloat JNICALL Java_javamsgque_MqS_ReadF
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadW
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_javamsgque_MqS_ReadW
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadD
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_javamsgque_MqS_ReadD
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadC
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_javamsgque_MqS_ReadC
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadB
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_javamsgque_MqS_ReadB
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadN
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_javamsgque_MqS_ReadN
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadBDY
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_javamsgque_MqS_ReadBDY
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadU
 * Signature: ()Ljavamsgque/MqBufferS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_ReadU
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadProxy
 * Signature: (Ljavamsgque/MqS;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadProxy
  (JNIEnv *, jobject, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadL_START
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadL_1START__
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadL_START
 * Signature: (Ljavamsgque/MqBufferS;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadL_1START__Ljavamsgque_MqBufferS_2
  (JNIEnv *, jobject, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadL_END
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadL_1END
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadT_START
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadT_1START__
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadT_START
 * Signature: (Ljavamsgque/MqBufferS;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadT_1START__Ljavamsgque_MqBufferS_2
  (JNIEnv *, jobject, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadT_END
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadT_1END
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadGetNumItems
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_javamsgque_MqS_ReadGetNumItems
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadItemExists
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_ReadItemExists
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    ReadUndo
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_ReadUndo
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendSTART
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendSTART
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendEND
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendEND
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    SendEND_AND_WAIT
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendEND_1AND_1WAIT__Ljava_lang_String_2
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    SendEND_AND_WAIT
 * Signature: (Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendEND_1AND_1WAIT__Ljava_lang_String_2I
  (JNIEnv *, jobject, jstring, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    SendEND_AND_CALLBACK
 * Signature: (Ljava/lang/String;Ljavamsgque/ICallback;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendEND_1AND_1CALLBACK
  (JNIEnv *, jobject, jstring, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendRETURN
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendRETURN
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendERROR
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendERROR
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendY
 * Signature: (B)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendY
  (JNIEnv *, jobject, jbyte);

/*
 * Class:     javamsgque_MqS
 * Method:    SendO
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendO
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     javamsgque_MqS
 * Method:    SendS
 * Signature: (S)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendS
  (JNIEnv *, jobject, jshort);

/*
 * Class:     javamsgque_MqS
 * Method:    SendI
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendI
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    SendF
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendF
  (JNIEnv *, jobject, jfloat);

/*
 * Class:     javamsgque_MqS
 * Method:    SendW
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendW
  (JNIEnv *, jobject, jlong);

/*
 * Class:     javamsgque_MqS
 * Method:    SendD
 * Signature: (D)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendD
  (JNIEnv *, jobject, jdouble);

/*
 * Class:     javamsgque_MqS
 * Method:    SendC
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendC
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    SendB
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendB
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     javamsgque_MqS
 * Method:    SendN
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendN
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     javamsgque_MqS
 * Method:    SendBDY
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendBDY
  (JNIEnv *, jobject, jbyteArray);

/*
 * Class:     javamsgque_MqS
 * Method:    SendU
 * Signature: (Ljavamsgque/MqBufferS;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendU
  (JNIEnv *, jobject, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendL_START
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendL_1START
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendL_END
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendL_1END
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SendT_START
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendT_1START
  (JNIEnv *, jobject, jstring);

/*
 * Class:     javamsgque_MqS
 * Method:    SendT_END
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SendT_1END
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveWorker
 * Signature: (I[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SlaveWorker
  (JNIEnv *, jobject, jint, jobjectArray);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveCreate
 * Signature: (ILjavamsgque/MqS;)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SlaveCreate
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveDelete
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_javamsgque_MqS_SlaveDelete
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveGet
 * Signature: (I)Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_SlaveGet
  (JNIEnv *, jobject, jint);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveGetMaster
 * Signature: ()Ljavamsgque/MqS;
 */
JNIEXPORT jobject JNICALL Java_javamsgque_MqS_SlaveGetMaster
  (JNIEnv *, jobject);

/*
 * Class:     javamsgque_MqS
 * Method:    SlaveIs
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_javamsgque_MqS_SlaveIs
  (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif
