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

  void MqC::Setup (void) {
    MqSetup();
    if (!strcmp(MqFactoryC<MqC>::DefaultIdent(),"libmsgque")) {
      MqFactoryC<MqC>::Default("ccmsgque");
    }
  }

  void MqC::Cleanup (void) {
    MqCleanup();
  }

  MqC** MqC::Resolve (MQ_CST ident, MQ_SIZE *retsize) {
    static MqThreadLocal MqC ** ret = NULL;
    static MqThreadLocal MQ_SIZE size = 0;

    struct MqS ** res = MqResolve(ident,retsize);
    MQ_SIZE num;

    if (ret == NULL) {
      ret = (MqC**) MqSysCalloc(MQ_ERROR_PANIC,10,sizeof(*ret));
      size = 10;
    }
    for (num=0; *res != NULL; res++, num++) {
      if (num+1 > size) {
	size+=10;
	ret = (MqC**) MqSysRealloc(MQ_ERROR_PANIC, ret, size*sizeof(*ret));
      }
      ret[num] = GetThis(*res);
    }
    ret[num] = NULL;
    return ret;
  }

  MqC::MqC (struct MqS * const tmpl) {
    MqContextInit (&context, 0, tmpl);
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

    // init the server interface
    IServerSetup * const iSetup = dynamic_cast<IServerSetup*const>(this);
    if (iSetup != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IServerSetup;
      ptr->call.ServerSetup = iSetup;
      MqConfigSetServerSetup (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, ProcCopy);
    }
    IServerCleanup * const iCleanup = dynamic_cast<IServerCleanup*const>(this);
    if (iCleanup != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IServerCleanup;
      ptr->call.ServerCleanup = iCleanup;
      MqConfigSetServerCleanup (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, ProcCopy);
    }

    // init the error interface
    IBgError * const iBgError = dynamic_cast<IBgError*const>(this);
    if (iBgError != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IBgError;
      ptr->call.BgError = iBgError;
      MqConfigSetBgError (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, ProcCopy);
    }

    // init the event interface
    IEvent * const iEvent = dynamic_cast<IEvent*const>(this);
    if (iEvent != NULL) {
      struct ProcCallS * ptr = (struct ProcCallS *) MqSysMalloc(MQ_ERROR_PANIC, sizeof(*ptr));
      ptr->type = ProcCallS::PC_IEvent;
      ptr->call.Event = iEvent;
      MqConfigSetEvent (&context, ProcCall, static_cast<MQ_PTR>(ptr), ProcFree, ProcCopy);
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
    p_message = MqSysStrDup(MQ_ERROR_PANIC, MqErrorGetText(context)); 
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
  
  enum MqErrorE MqC::ErrorSet (const exception& e) {
    MqCException const * const  mqex = dynamic_cast<MqCException const * const>(&e);
    if (mqex != NULL) {
      return MqErrorSet (&context, mqex->num(), mqex->code(), mqex->what(), NULL);
    } else {
      return MqErrorC (&context, __func__, -1, e.what());
    }
  }
} // END - namespace "ccmsgque"

#if defined(_MSC_VER)

BOOL WINAPI DllMain(
    HINSTANCE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
  switch (ul_reason_for_call)
  {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      return FALSE;
    case DLL_PROCESS_ATTACH:
      ccmsgque::MqC::Setup();
      break;
    case DLL_PROCESS_DETACH:
      ccmsgque::MqC::Cleanup();
      break;
  }
  return TRUE;
}
#endif
