/**
 *  \file       theLink/rubymsgque/MqS_ruby.c
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

VALUE cMqS;
VALUE globalRef;

/*****************************************************************************/
/*                                                                           */
/*                                  MqS                                      */
/*                                                                           */
/*****************************************************************************/

/*
static void sMqMark (MQ_PTR data)
{
printP(data)
  rb_gc_mark((VALUE)data);
}
*/

static void sMark (void * ctx)
{
  struct MqS *mqctx = (struct MqS *) ctx;
  MqMark(mqctx, (MqMarkF)rb_gc_mark);
}

static void sFree (void * ctx)
{
  struct MqS *mqctx = (struct MqS *) ctx;
  mqctx->setup.factory = NULL;
  mqctx->setup.Event.fCall = NULL;
  mqctx->self = NULL;
  // delete the context
  MqContextDelete(&mqctx);
}

static enum MqErrorE sEvent (
  struct MqS * const context,
  MQ_PTR const data
)
{
  rb_thread_schedule();
  return MQ_OK;
}


static VALUE new(int argc, VALUE *argv, VALUE class)
{
  struct MqS * mqctx = NULL;
  struct MqS * tmpl = NULL;

  if (argc < 0 || argc > 1) rb_raise(rb_eArgError, "usage: new(?MqS-Type-Arg?)");

  if (argc == 1 && !NIL_P(argv[0]) ) {
    CheckType(argv[0], cMqS, "usage: new(?MqS-Type-Arg?)");
    tmpl = VAL2MqS(argv[0]);
    //argc-=1;
    //argv+=1;
  }

  mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), tmpl);

  // create a "ruby" object and link it to the "mqctx" class
  VALUE self = Data_Wrap_Struct(class, sMark, sFree, mqctx);

  // create ruby command
  mqctx->self = (void*) self;

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;      
  MqConfigSetIgnoreThread(mqctx, MQ_YES);
  MqConfigSetEvent(mqctx,sEvent,NULL,NULL,NULL);

  // call "initialize"
  rb_obj_call_init(self, argc, argv);

  //MqConfigSetEvent (MQCTX, EventLink, NULL, NULL, NULL);
  
  return self;
}

static VALUE Exit(VALUE self)
{
  MqExit(MQCTX);
  return Qnil;
}

static VALUE Delete(VALUE self)
{
  MqContextFree (MQCTX);
  return Qnil;
}

static VALUE LogC(VALUE self, VALUE prefix, VALUE level, VALUE str)
{
  MqLogC(MQCTX, VAL2CST(prefix), VAL2INT(level), VAL2CST(str));
  return Qnil;
}

static VALUE Sleep(VALUE self, VALUE sec)
{
  MqSysSleep(MQCTX, VAL2INT(sec));
  return Qnil;
}

static VALUE USleep(VALUE self, VALUE usec)
{
  MqSysUSleep(MQCTX, VAL2INT(usec));
  return Qnil;
}

static VALUE Init(int argc, VALUE *argv, VALUE self)
{
  NS(argv2bufl) (NULL, MqInitCreate(), argc, argv);
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Init) (void) {

  // define class MqS
  cMqS = rb_define_class("MqS", rb_cObject);

  rb_define_singleton_method(cMqS, "new",  new,  -1);
  rb_define_method(cMqS, "Init", Init, -1);

  rb_define_method(cMqS, "Exit",    Exit,   0);
  rb_define_method(cMqS, "Delete",  Delete, 0);
  rb_define_method(cMqS, "LogC",    LogC,   3);
  rb_define_method(cMqS, "Sleep",   Sleep,  1);
  rb_define_method(cMqS, "USleep",  USleep, 1);

  // Timeout
  rb_define_const(cMqS, "TIMEOUT_DEFAULT",  INT2VAL(-1));
  rb_define_const(cMqS, "TIMEOUT_USER",     INT2VAL(-2));
  rb_define_const(cMqS, "TIMEOUT_MAX",      INT2VAL(-3));

  // Wait
  rb_define_const(cMqS, "WAIT_NO",	    INT2VAL(0));
  rb_define_const(cMqS, "WAIT_ONCE",	    INT2VAL(1));
  rb_define_const(cMqS, "WAIT_FOREVER",     INT2VAL(2));

  // Start
  rb_define_const(cMqS, "START_DEFAULT",    INT2VAL(0));
  rb_define_const(cMqS, "START_FORK",	    INT2VAL(1));
  rb_define_const(cMqS, "START_THREAD",	    INT2VAL(2));
  rb_define_const(cMqS, "START_SPAWN",	    INT2VAL(3));

  NS(MqS_Sys_Init)();
  NS(MqS_Send_Init)();
  NS(MqS_Read_Init)();
  NS(MqS_Error_Init)();
  NS(MqS_Config_Init)();
  NS(MqS_Service_Init)();
  NS(MqS_Link_Init)();
  NS(MqS_Slave_Init)();
  NS(MqS_Factory_Init)();

  globalRef = rb_ary_new();
  rb_gc_register_mark_object(globalRef);
}

