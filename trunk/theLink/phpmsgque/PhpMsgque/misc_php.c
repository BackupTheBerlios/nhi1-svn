/**
 *  \file       theLink/phpmsgque/PhpMsgque/misc_php.c
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include "msgque_php.h"

/*****************************************************************************/
/*                                                                           */
/*                                private                                    */
/*                                                                           */
/*****************************************************************************/

static int MqBufferLAppendZValArrayItem (zval **, MQ_BFL TSRMLS_DC);

void MqBufferLAppendZVal(MQ_BFL bufL, zval* arg TSRMLS_DC) {
  switch (Z_TYPE_P(arg)) {
    case IS_STRING:
      MqBufferLAppendC(bufL, Z_STRVAL_P(arg));
      break;
    case IS_BOOL:
      MqBufferLAppendO(bufL, Z_LVAL_P(arg) ? 1 : 0);
      break;
    case IS_LONG:
      MqBufferLAppendI(bufL, Z_LVAL_P(arg));
      break;
    case IS_DOUBLE:
      MqBufferLAppendD(bufL, Z_DVAL_P(arg));
      break;
    case IS_ARRAY:
      zend_hash_apply_with_argument(Z_ARRVAL_P(arg), 
	(apply_func_arg_t) MqBufferLAppendZValArrayItem, bufL TSRMLS_CC);
      break;
    default:
      RaiseError("invalid argument type");
      break;
  }
}

static int MqBufferLAppendZValArrayItem (zval **argP, MQ_BFL args TSRMLS_DC) {
  MqBufferLAppendZVal (args, *argP TSRMLS_CC);
  return ZEND_HASH_APPLY_KEEP;
}

MQ_BFL NS(Argument2MqBufferLS)(const int numArgs TSRMLS_DC)
{
  struct MqBufferLS * args = NULL;

  if (numArgs > 0) {
    int i;
    zval ***arguments;
    args = MqBufferLCreate(numArgs);
    arguments = emalloc(sizeof(zval**) * numArgs);
    if (zend_get_parameters_array_ex(numArgs,arguments)) {
      ZEND_WRONG_PARAM_COUNT_WITH_RETVAL(args);
    }
    for (i=0; i<numArgs; i++) {
      MqBufferLAppendZVal (args, *arguments[i] TSRMLS_CC);
    }
    efree(arguments);
  }

  return args;
}


/*****************************************************************************/
/*                                                                           */
/*                                public                                     */
/*                                                                           */
/*****************************************************************************/

void NS(Misc_Init)(TSRMLS_D) {
}

