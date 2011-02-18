/**
 *  \file       theLink/rubymsgque/MqDumpS_ruby.c
 *  \brief      \$Id$
 *  
 *  (C) 2011 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_ruby.h"

VALUE cMqDumpS;

#define DUMP	      VAL2MqDumpS(self)
#define SETUP_dump    struct MqDumpS *dump = DUMP

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static VALUE Size (VALUE self) {
  return INT2VAL(MqDumpSize(DUMP));
}

static void Free (void* data) {
  MqSysFree(data);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

VALUE NS(MqDumpS_New) (struct MqDumpS *dump) {
  return Data_Wrap_Struct(cMqDumpS, NULL, Free, dump);
}

void NS(MqDumpS_Init) (void) {
  cMqDumpS = rb_define_class("MqDumpS", rb_cObject);

  // make class note create-able
  rb_funcall(cMqDumpS, rb_intern("private_class_method"), 1, CST2VAL("new"));

  // method
  rb_define_method(cMqDumpS, "Size", Size, 0);
}

