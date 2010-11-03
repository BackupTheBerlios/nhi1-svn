
#include "msgque_php.h"

zend_class_entry *NS(MqBufferS);

#define BUF	      VAL2MqBufferS(getThis())
#define CTX	      buf->context
#define SETUP_buf     MQ_BUF buf = BUF
#define ErrorBufToPhp() NS(MqSException_Raise)(buf->context TSRMLS_CC)
#define ErrorBufToPhpWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    ErrorBufToPhp(); \
  }

#define GET(T,M) \
static \
PHP_METHOD(PhpMsgque_MqBufferS, Get ## T) \
{ \
  SETUP_buf; \
  MQ_ ## M ret; \
  ErrorBufToPhpWithCheck(MqBufferGet ## T (buf, &ret)); \
  M ## 2VAL (return_value,ret); \
}

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

GET(Y,BYT);
GET(O,BOL);
GET(S,SRT);
GET(I,INT);
GET(W,WID);
GET(F,FLT);
GET(D,DBL);
GET(C,CST);

/*
static VALUE GetB (VALUE self) {
  MQ_BIN val;
  MQ_SIZE len;
  SETUP_buf
  ErrorBufToPhpWithCheck(MqBufferGetB(buf, &val, &len));
  return BIN2VAL(val,len);
}
*/

static
PHP_METHOD(PhpMsgque_MqBufferS, GetType)
{
  MQ_STRB const str[2] = {MqBufferGetType(BUF), '\0'};
  CST2VAL(return_value, str);
}

static
PHP_METHOD(PhpMsgque_MqBufferS, __construct)
{
  RaiseError("it is not allowed to create an instance og 'MqBufferS'.");
}

ZEND_BEGIN_ARG_INFO_EX(no_arg, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry NS(MqBufferS_functions)[] = {
  PHP_ME(PhpMsgque_MqBufferS, __construct,	NULL,	      ZEND_ACC_PRIVATE|ZEND_ACC_CTOR)
  PHP_ME(PhpMsgque_MqBufferS, GetY,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetO,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetS,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetI,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetW,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetF,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetD,		no_arg,	      ZEND_ACC_PUBLIC)
  PHP_ME(PhpMsgque_MqBufferS, GetC,		no_arg,	      ZEND_ACC_PUBLIC)

  {NULL, NULL, NULL}
};

/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(MqBufferS_New) (zval *return_value, MQ_BUF buf TSRMLS_DC)
{
  // convert to an object instance
  if (object_init_ex(return_value, NS(MqBufferS)) == FAILURE) {
    RaiseError("unable to create an 'MqBufferS' instance.");
  }
  // link 'buf' with the object instance
  zend_update_property_long(NS(MqBufferS), return_value, ID(__buf), (long) buf TSRMLS_CC);
  return;
}

void NS(MqBufferS_Init) (TSRMLS_D) {
  zend_class_entry me_ce;

  // create class and make depend on "Exception"
  INIT_CLASS_ENTRY(me_ce,"MqBufferS", NS(MqBufferS_functions));
  NS(MqBufferS) = zend_register_internal_class(&me_ce TSRMLS_CC);

  // define additional properties "buf" to save the "struct MqBufferS *" pointer
  zend_declare_property_null(NS(MqBufferS), ID(__buf), ZEND_ACC_PRIVATE TSRMLS_CC);
}

