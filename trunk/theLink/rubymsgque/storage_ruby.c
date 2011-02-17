/**
 */

#include "msgque_ruby.h"

extern VALUE cMqS;

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static VALUE StorageOpen (VALUE self, VALUE storageFile) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqStorageOpen(mqctx, VAL2CST(storageFile)));
  return Qnil;
}

static VALUE StorageClose (VALUE self) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqStorageClose(mqctx));
  return Qnil;
}

static VALUE StorageInsert (VALUE self) {
  MQ_TRA ret;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqStorageInsert(mqctx, &ret));
  return WID2VAL(ret);
}

static VALUE StorageSelect (int argc, VALUE *argv, VALUE self) {
  MQ_TRA ret = 0L;
  SETUP_mqctx
  if (argc < 0 || argc > 2) {
    goto error;
  } else if (argc == 1) {
    ret = VAL2WID(argv[0]);
  }
  ErrorMqToRubyWithCheck(MqStorageSelect(mqctx, &ret));
  return WID2VAL(ret);
error:
  rb_raise(rb_eArgError,"usage: StorageSelect(?transLId?)");
}

static VALUE StorageDelete (VALUE self, VALUE id) {
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqStorageDelete(mqctx, VAL2WID(id)));
  return Qnil;
}

static VALUE StorageCount (VALUE self) {
  MQ_TRA count;
  SETUP_mqctx
  ErrorMqToRubyWithCheck(MqStorageCount(mqctx, &count));
  return WID2VAL(count);
}

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqS_Storage_Init)(void) {
  rb_define_method(cMqS, "StorageOpen",	  StorageOpen,	  1);
  rb_define_method(cMqS, "StorageClose",  StorageClose,	  0);
  rb_define_method(cMqS, "StorageInsert", StorageInsert,  0);
  rb_define_method(cMqS, "StorageSelect", StorageSelect,  -1);
  rb_define_method(cMqS, "StorageDelete", StorageDelete,  1);
  rb_define_method(cMqS, "StorageCount",  StorageCount,	  0);
}

