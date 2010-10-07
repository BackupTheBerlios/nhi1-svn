/**
 *  \file       theLink/rubymsgque/link_ruby.c
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

static VALUE LinkCreate(int argc, VALUE *argv, VALUE self)
{
  SETUP_mqctx
  struct MqBufferLS * args = NS(argv2bufl)(argc,argv);

//printXULS(NULL, args);

  // create Context
  ErrorMqToRubyWithCheck (MqLinkCreate(mqctx, &args));
  return Qnil;
}

static VALUE LinkCreateChild(int argc, VALUE *argv, VALUE self)
{
  struct MqS * parent;
  struct MqBufferLS * args = NULL;
  SETUP_mqctx

  // get parent
  if (argc < 1) rb_raise(rb_eArgError, "usage: LinkCreateChild parent ...");
  CheckType(argv[0], cMqS, usage: LinkCreateChild MqS-Type-Arg ...);
  parent = VAL2MqS(argv[0]);
  argc--; argv++;

  // command-line arguments to MqBufferLS
  args = NS(argv2bufl)(argc,argv);

  // create Context
  ErrorMqToRubyWithCheck (MqLinkCreateChild(mqctx, parent, &args));
  return Qnil;
}

static VALUE LinkDelete(VALUE self)
{
  MqLinkDelete(MQCTX);
  return Qnil;
}

static VALUE LinkConnect(VALUE self)
{
  SETUP_mqctx
  ErrorMqToRubyWithCheck (MqLinkConnect (mqctx));
  return Qnil;
}

static VALUE LinkGetParent (VALUE self) {
  struct MqS * const parent = MqLinkGetParentI(MQCTX);
  return MqS2VAL(parent);
}

static VALUE LinkIsParent(VALUE self)
{
  return BOL2VAL(MqLinkIsParent(MQCTX));
}

static VALUE LinkGetCtxId(VALUE self)
{
  return INT2VAL(MqLinkGetCtxId(MQCTX));
}

static VALUE LinkIsConnected(VALUE self)
{
  return BOL2VAL(MqLinkIsConnected(MQCTX));
}

static VALUE LinkGetTargetIdent(VALUE self)
{
  MQ_CST ident;
  ident = MqLinkGetTargetIdent(MQCTX);
  return CST2VAL(ident);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Link_Init)(void) {
  rb_define_method(cMqS, "LinkCreate",		LinkCreate,	    -1);
  rb_define_method(cMqS, "LinkCreateChild",	LinkCreateChild,    -1);
  rb_define_method(cMqS, "LinkDelete",		LinkDelete,	    0);
  rb_define_method(cMqS, "LinkConnect",		LinkConnect,	    0);
  rb_define_method(cMqS, "LinkGetParent",	LinkGetParent,	    0);
  rb_define_method(cMqS, "LinkIsParent",	LinkIsParent,	    0);
  rb_define_method(cMqS, "LinkGetCtxId",	LinkGetCtxId,	    0);
  rb_define_method(cMqS, "LinkIsConnected",	LinkIsConnected,    0);
  rb_define_method(cMqS, "LinkGetTargetIdent",	LinkGetTargetIdent, 0);
}

