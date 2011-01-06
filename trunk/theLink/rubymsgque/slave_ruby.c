/**
 *  \file       theLink/rubymsgque/slave_ruby.c
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

extern VALUE cMqS;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static VALUE SlaveWorker(int argc, VALUE *argv, VALUE self)
{
  MQ_BFL args = NULL;
  SETUP_mqctx
  MQ_INT id;

  // get parent
  if (argc < 1) rb_raise(rb_eArgError, "usage: SlaveWorker id ...");
  id = VAL2INT(argv[0]);
  argc--; argv++;

  // command-line arguments to MqBufferLS
  args = NS(argv2bufl)(NULL,NULL,argc,argv);

  // create Worker
  ErrorMqToRubyWithCheck (MqSlaveWorker(mqctx, id, &args));
  return Qnil;
}

static VALUE SlaveCreate(VALUE self, VALUE id, VALUE mqs)
{
  SETUP_mqctx
  CheckType(mqs, cMqS, "usage: SlaveCreate id MqS-Type-Arg");
  ErrorMqToRubyWithCheck (MqSlaveCreate(mqctx, VAL2INT(id), VAL2MqS(mqs)));
  return Qnil;
}

static VALUE SlaveDelete(VALUE self, VALUE id)
{
  SETUP_mqctx
  ErrorMqToRubyWithCheck (MqSlaveDelete(mqctx, VAL2INT(id)));
  return Qnil;
}

static VALUE SlaveGet(VALUE self, VALUE id)
{
  return MqS2VAL(MqSlaveGet(MQCTX,VAL2INT(id)));
}

static VALUE SlaveGetMaster(VALUE self)
{
  return MqS2VAL(MqSlaveGetMaster(MQCTX));
}

static VALUE SlaveIs(VALUE self)
{
  return BOL2VAL(MqSlaveIs(MQCTX));
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Slave_Init)(void) {
  rb_define_method(cMqS, "SlaveWorker",		SlaveWorker,	    -1);
  rb_define_method(cMqS, "SlaveCreate",		SlaveCreate,	    2);
  rb_define_method(cMqS, "SlaveDelete",		SlaveDelete,	    1);
  rb_define_method(cMqS, "SlaveGet",		SlaveGet,	    1);
  rb_define_method(cMqS, "SlaveGetMaster",	SlaveGetMaster,	    0);
  rb_define_method(cMqS, "SlaveIs",		SlaveIs,	    0);
}

