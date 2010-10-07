/**
 *  \file       theLink/rubymsgque/context_ruby.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */
#include "msgque_ruby.h"

#ifdef MQ_SHARED_IS_DLL
#    define RUBYMQ_EXTERN __declspec(dllexport)
#else
#    define RUBYMQ_EXTERN __attribute__ ((visibility("default")))
#endif

RUBYMQ_EXTERN void Init_rubymsgque() {

  // Initialize components
  NS(MqS_Init)();
  NS(MqSException_Init)();
  NS(MqBufferS_Init)();

  // get the script name
  VALUE a0 = rb_gv_get("$0");

  // init libmsgque global data
  if (MqInitBuf == NULL && a0 != Qnil) {
    struct MqBufferLS * initB = MqInitCreate();
    MqBufferLAppendC(initB, "ruby");
    MqBufferLAppendC(initB, RSTRING_PTR(a0));
  }
}
