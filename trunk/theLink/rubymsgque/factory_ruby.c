/**
 *  \file       theLink/rubymsgque/factory_ruby.c
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

extern VALUE cMqS;

VALUE cMqFactoryS;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

#define ErrorFactoryToRubyWithCheck(p) { \
  enum MqFactoryReturnE ret; \
  if ((ret = (p)) != MQ_FACTORY_RETURN_OK) { \
    rb_raise(rb_eRuntimeError, MqFactoryErrorMsg(ret)); \
    return Qnil; \
  } \
}

static VALUE sFactoryError (VALUE vtmpl, VALUE ex) {
  if (NIL_P(vtmpl)) {
    rb_exc_raise(ex);
  } else {
    NS(MqSException_Set) (VAL2MqS(vtmpl), ex);
  }
  return Qnil;
}

static VALUE sFactoryCall (VALUE args) {
  VALUE *argsP = RARRAY_PTR(args);
  return rb_method_call(1, argsP, argsP[1]);
}

static enum MqErrorE
FactoryCreate(
  struct MqS * const tmpl,
  enum MqFactoryE create,
  struct MqFactoryS *item,
  struct MqS  ** mqctxP
)
{
  enum MqErrorE mqret = MQ_OK;
  struct MqS * mqctx = NULL;
  VALUE args;
  VALUE vtmpl;
  VALUE self;

/*
  //struct MqS * const mqctx = tmpl;
  if (create == MQ_FACTORY_NEW_FORK) {
    //rb_thread_atfork();
  }
  if (create == MQ_FACTORY_NEW_THREAD) {
    //RUBY_INIT_STACK;
  }
*/

  // setup templ
  vtmpl = MqS2VAL(tmpl);

  // setup arguments
  args = rb_ary_new3(2, vtmpl, PTR2VAL(item->Create.data));

  // call "new"
  self = rb_rescue2(sFactoryCall, args, sFactoryError, vtmpl, rb_eException, (VALUE)0);

  if (NIL_P(self)) {
    if (create == MQ_FACTORY_NEW_INIT) {
      mqret = MQ_ERROR;
    } else {
      mqret = MqErrorGetCode(tmpl);
    }
  } else {
    mqctx = VAL2MqS(self);
    //rb_gc_register_address((VALUE*)&mqctx->self);
    INCR_REG(self);
    if (create == MQ_FACTORY_NEW_INIT) {
      MqSetupDup(mqctx, tmpl);
    }
  }
  
  *mqctxP = mqctx;
  return mqret;
}

static void
FactoryDelete(
  struct MqS * mqctx,
  MQ_BOL doFactoryDelete,
  struct MqFactoryS * const item
)
{
  MqContextFree (mqctx);
  if (doFactoryDelete && mqctx->self != NULL) {
    //rb_gc_unregister_address((VALUE*)&mqctx->self);
    DECR_REG((VALUE)mqctx->self);
    mqctx->self = NULL;
  }
}

void FactoryFree (
  MQ_PTR *dataP
)
{
  if (*dataP != NULL) {
    DECR_REF((VALUE) *dataP);
  }
  *dataP = NULL;
}

void FactoryCopy (
  MQ_PTR *dataP
)
{
  if (*dataP != NULL) {
    INCR_REF((VALUE) *dataP);
  }
}

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

static VALUE FactoryCtxIdentGet (VALUE self)
{
  SETUP_mqctx;
  return CST2VAL(MqFactoryCtxIdentGet (mqctx));
}

static VALUE FactoryCtxIdentSet (VALUE self, VALUE ident)
{
  SETUP_mqctx;
  ErrorMqToRubyWithCheck(MqFactoryCtxIdentSet(mqctx, (MQ_CST) (VAL2CST(ident))));
  return Qnil;
}

static VALUE FactoryCtxDefaultSet (VALUE self, VALUE ident)
{
  SETUP_mqctx;
  ErrorMqToRubyWithCheck(MqFactoryCtxDefaultSet(mqctx, (MQ_CST) (VAL2CST(ident))));
  return Qnil;
}

/*****************************************************************************/
/*                                                                           */
/*                                static                                     */
/*                                                                           */
/*****************************************************************************/

static VALUE FactoryAdd (int argc, VALUE *argv, VALUE self)
{
  VALUE meth, ident, class;

  if (argc < 1 || argc > 2) rb_raise(rb_eArgError, "usage: FactoryAdd(?ident?, class)");

  ident = argv[0];
  class = argc == 1 ? argv[0] : argv[1];
  CheckType(class, rb_cClass, "usage: FactoryAdd(?ident?, class)");
  meth = rb_obj_method(class, CST2VAL("new"));

  INCR_REF2(meth);
  ErrorFactoryToRubyWithCheck(
    MqFactoryAdd(VAL2CST(ident), FactoryCreate, (MQ_PTR)meth, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL)
  );

  return Qnil;
}

static VALUE FactoryDefault (int argc, VALUE *argv, VALUE self)
{
  VALUE meth, ident, class;

  if (argc < 1 || argc > 2) rb_raise(rb_eArgError, "usage: FactoryDefault(ident, ?class?)");

  ident = argv[0];
  class = argc == 1 ? cMqS : argv[1];
  CheckType(class, rb_cClass, "usage: FactoryDefault(ident, ?class?)");
  meth = rb_obj_method(class, CST2VAL("new"));

  INCR_REF2(meth);
  ErrorFactoryToRubyWithCheck(
    MqFactoryDefault(VAL2CST(ident), FactoryCreate, (MQ_PTR)meth, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL)
  );

  return Qnil;
}

static VALUE FactoryDefaultIdent (VALUE self)
{
  return CST2VAL(MqFactoryDefaultIdent());
}

static VALUE FactoryNew (int argc, VALUE *argv, VALUE self)
{
  struct MqS *mqctx = NULL;
  VALUE meth, ident, class;

  if (argc < 1 || argc > 2) rb_raise(rb_eArgError, "usage: FactoryNew(?ident?, class)");

  ident = argv[0];
  class = argc == 1 ? argv[0] : argv[1];
  CheckType(class, rb_cClass, "usage: FactoryNew(?ident?, class)");
  meth = rb_obj_method(class, CST2VAL("new"));

  INCR_REF2(meth);
  ErrorFactoryToRubyWithCheck(
    MqFactoryNew(VAL2CST(ident), FactoryCreate, (MQ_PTR)meth, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL, NULL, &mqctx)
  );

  return MqS2VAL(mqctx);
}

static VALUE FactoryCall (VALUE self, VALUE ident)
{
  struct MqS *mqctx = NULL;
  ErrorFactoryToRubyWithCheck(MqFactoryCall(VAL2CST(ident), NULL, &mqctx));
  return MqS2VAL(mqctx);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Factory_Init)(void) {
  VALUE meth = rb_obj_method(cMqS, CST2VAL("new"));

  rb_define_method(cMqS, "FactoryCtxIdentGet",	  FactoryCtxIdentGet,	0);
  rb_define_method(cMqS, "FactoryCtxIdentSet",	  FactoryCtxIdentSet,	1);
  rb_define_method(cMqS, "FactoryCtxDefaultSet",  FactoryCtxDefaultSet,	1);

  rb_define_global_function("FactoryAdd",	    FactoryAdd,		  -1);
  rb_define_global_function("FactoryDefault",	    FactoryDefault,	  -1);
  rb_define_global_function("FactoryDefaultIdent",  FactoryDefaultIdent,  0);
  rb_define_global_function("FactoryNew",	    FactoryNew,		  -1);
  rb_define_global_function("FactoryCall",	    FactoryCall,	  1);
  
  // Initialize "default" factory
  INCR_REF2(meth);
  MqFactoryDefault("rubymsgque", FactoryCreate, (MQ_PTR)meth, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL);
}

