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

VALUE id_receiver;

#ifdef HAVE_FORK
//static id_t NS(fork) (void) {
//  int status;
//  return rb_fork(&status, NULL, 0, Qnil)
//}
#endif

RUBYMQ_EXTERN void Init_rubymsgque() {

  // Register system
#ifdef HAVE_FORK
//  MqInitSysAPI(NS(fork),NULL);
#endif

  // Initialize components
  NS(MqS_Init)();
  NS(MqSException_Init)();
  NS(MqBufferS_Init)();

  // get the script name
  VALUE a0 = rb_gv_get("$0");

  // init libmsgque global data
  if (MqInitBuf == NULL && a0 != Qnil) {
    struct MqBufferLS * initB = MqInitCreate();
    MqBufferLAppendC(initB, VAL2CST(rb_argv0));
    MqBufferLAppendC(initB, VAL2CST(a0));
  }

  // set global data
  id_receiver = rb_intern("receiver");
}

