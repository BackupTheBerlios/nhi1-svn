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

#define MQ_CONTEXT_S mqctx

VALUE cMqS;

/*****************************************************************************/
/*                                                                           */
/*                                  MqS                                      */
/*                                                                           */
/*****************************************************************************/

static void ProcessExit (MQ_INT num)
{
  rb_exit(num);
}

static void ThreadExit (MQ_INT num)
{
}

/*
static void ForkAndSpawnInit (struct MqS *mqctx) {
  rb_thread_atfork_before_exec();
}
*/

static void Free (void * ctx)
{
  struct MqS *mqctx = (struct MqS *) ctx;
  mqctx->setup.Factory.Delete.fCall = NULL;
  mqctx->setup.Event.fCall = NULL;
  mqctx->self = NULL;
  // delete the context
  MqContextDelete(&mqctx);
}

static VALUE new(VALUE class)
{
  struct MqS * mqctx = (struct MqS *) MqContextCreate(sizeof (*mqctx), NULL);

  // create a "ruby" object and link it to the "mqctx" class
  VALUE self = Data_Wrap_Struct(class, NULL, Free, mqctx);

  // create ruby command
  mqctx->self = (void*) self;

  // set configuration data
  mqctx->setup.Child.fCreate   = MqLinkDefault;
  mqctx->setup.Parent.fCreate  = MqLinkDefault;      
  mqctx->setup.fProcessExit    = ProcessExit;    
  mqctx->setup.fThreadExit     = ThreadExit;     
  //mqctx->setup.fSpawnInit      = ForkAndSpawnInit;     
  //mqctx->setup.fForkInit       = ForkAndSpawnInit;     

  // call "initialize"
  rb_obj_call_init(self, 0, NULL);

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

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Init) (void) {

  // define class MqS
  cMqS = rb_define_class("MqS", rb_cObject);

  rb_define_singleton_method(cMqS, "new", new, 0);

  rb_define_method(cMqS, "Exit",    Exit,   0);
  rb_define_method(cMqS, "Delete",  Delete, 0);
  rb_define_method(cMqS, "LogC",    LogC,   3);

  NS(MqS_Send_Init)();
  NS(MqS_Read_Init)();
  NS(MqS_Error_Init)();
  NS(MqS_Config_Init)();
  NS(MqS_Service_Init)();
  NS(MqS_Link_Init)();
  NS(MqS_Slave_Init)();
}

