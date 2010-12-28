/**
 *  \file       theLink/ccmsgque/ccmsgque.cc
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <typeinfo> 
#include <stdexcept>

#include "ccmsgque_private.h"
#define MQ_CONTEXT_S &context

namespace ccmsgque {

  void MqC::libInit (void) {
    MqFactoryC<MqC>::Default("ccmsgque");
  }

  MqC::MqC (MqC& tmpl) {
    MqContextInit (&context, 0, &tmpl.context);
    MqConfigSetSelf (&context, this);
  }

  MqC::MqC () {
    MqContextInit (&context, 0, NULL);
    MqConfigSetSelf (&context, this);
  }

  MqC::~MqC () {
    MqContextFree (&context);
  }

  void MqC::objInit () {
    // use "context.setup.Parent.fCreate" to ckeck in context was initialized
    if (context.setup.Parent.fCreate != NULL) return;

    context.setup.Parent.fCreate = MqLinkDefault;
    context.setup.Child.fCreate = MqLinkDefault;

    // init the "factory" interface
/*
    IFactory *const iFactory = dynamic_cast<IFactory*const>(this);
    if (iFactory != NULL) {
      context.setup.Factory.Create.fCall = FactoryCreate;
      context.setup.Factory.Create.data = iFactory;
    }
    // required -> slave-Z-ERR-1-
    context.setup.Factory.Delete.fCall = FactoryDelete;
*/

    // init the server interface
    IServerSetup * const iSetup = dynamic_cast<IServerSetup*const>(this);
    if (iSetup != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IServerSetup;
      ptr->call.ServerSetup = iSetup;
      MqConfigSetServerSetup (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, NULL);
    }
    IServerCleanup * const iCleanup = dynamic_cast<IServerCleanup*const>(this);
    if (iCleanup != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IServerCleanup;
      ptr->call.ServerCleanup = iCleanup;
      MqConfigSetServerCleanup (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, NULL);
    }

    // init the error interface
    IBgError * const iBgError = dynamic_cast<IBgError*const>(this);
    if (iBgError != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IBgError;
      ptr->call.BgError = iBgError;
      MqConfigSetBgError (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, NULL);
    }

    // init the event interface
    IEvent * const iEvent = dynamic_cast<IEvent*const>(this);
    if (iEvent != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IEvent;
      ptr->call.Event = iEvent;
      MqConfigSetEvent (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, NULL);
    }
  }

  void MqC::LinkCreate (
    struct MqBufferLS * argv
  )
  {
    objInit ();
    ErrorCheck(MqLinkCreate (&context, &argv));
  }

  void MqC::_LinkCreateV (int dummy, ...)
  {
    va_list ap;
    struct MqBufferLS * args;

    va_start (ap, dummy);
    args = MqBufferLCreateArgsVA (&context, ap);
    va_end (ap);

    LinkCreate (args);
  }
  
  void MqC::LinkCreateChild (
    MqC&	parent,
    struct MqBufferLS * args
  )
  {
    objInit ();
    ErrorCheck (MqLinkCreateChild(&context, &parent.context, &args));
  }

  void MqC::_LinkCreateChildV (
    MqC&	parent,
    int         dummy,
    ...
  )
  {
    va_list ap;
    struct MqBufferLS * args;

    va_start (ap, dummy);
    args = MqBufferLCreateArgsVA (&context, ap);
    va_end (ap);

    LinkCreateChild(parent, args);
  }

  MqCException::MqCException(struct MqS *const context) {
    p_message = mq_strdup(MqErrorGetText(context)); 
    p_num = MqErrorGetNumI(context);
    p_code = MqErrorGetCodeI(context);
    MqErrorReset (context);
  }

  MqCException::MqCException(int mynum, enum MqErrorE mycode, MQ_CST myfunc, MQ_CST myfile, const int myline, MQ_CST mytxt) {
    const int len = (strlen(mytxt)+100);
    char * fn = MqSysBasename(myfile, MQ_YES);
    p_num = mynum;
    p_code = mycode;
    p_message = (MQ_STR) MqSysMalloc (MQ_ERROR_PANIC, len);
    mq_snprintf(p_message,len,"Error at %s(%s:%i) : %s", myfunc, fn , myline, mytxt);
    MqSysFree(fn);
  }

  void MqC::SlaveWorkerV (
    int const	master_id,
    ...
  )
  {
    va_list ap;
    struct MqBufferLS * args;

    va_start (ap, master_id);
    args = MqBufferLCreateArgsVA(&context, ap);
    va_end (ap);

    SlaveWorker (master_id, args);
  }

} // END - namespace "ccmsgque"

