/**
 *  \file       theLink/ccmsgque/ccservice.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

/*****************************************************************************/
/*                                                                           */
/*                                context                                    */
/*                                                                           */
/*****************************************************************************/

#include "ccmsgque_private.h"

namespace ccmsgque {

  void MqC::ServiceCreate(
    MQ_CST const	  token,
    CallbackF const	  callback
  ) throw(MqCException)
  {
    struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
    ptr->type = ProcCallS::PC_CallbackF;
    ptr->call.Callback = callback;
    ErrorCheck (MqServiceCreate(&context, token, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree));
  }

  void MqC::ServiceCreate(
    MQ_CST const	  token,
    IService * const	  service
  ) throw(MqCException)
  {
    struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
    ptr->type = ProcCallS::PC_IService;
    ptr->call.Service = service;
    ErrorCheck (MqServiceCreate(&context, token, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree));
  }

  void MqC::ServiceDelete(
    MQ_CST const	  token
  ) throw(MqCException)
  {
    ErrorCheck (MqServiceDelete(&context, token));
  }

} // END - namespace "ccmsgque"


