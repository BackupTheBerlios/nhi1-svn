
#if defined(VERSION)
#undef VERSION
#endif

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"
#include "mqconfig.h"
#include "msgque.h"
#include "debug.h"

#ifndef USE_64_BIT_INT
#error perl compile option "USE_64_BIT_INT" is required
#endif

#define MqErrorSys(cmd) \
  MqErrorV (context, __func__, errno, \
    "can not '%s' -> ERR<%s>", MQ_CPPXSTR(cmd), strerror (errno))

#define ErrorMqToPerlWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    SV* errsv = get_sv("@", TRUE); \
    MqSException *ex = (MqSException*) MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(*ex)); \
    ex->num = MqErrorGetNum(context); \
    ex->code = MqErrorGetCodeI(context); \
    ex->text = MqSysStrDup(MQ_ERROR_PRINT, MqErrorGetText(context)); \
    sv_setref_pv(errsv, "Net::PerlMsgque::MqSException", (void*)ex); \
    MqErrorReset(context); \
    croak(NULL); \
  }

#define ErrorBufferToPerlWithCheck(PROC) \
  if (unlikely(MqErrorCheckI(PROC))) { \
    SV* errsv = get_sv("@", TRUE); \
    struct MqS * const context = buffer->context; \
    MqSException *ex = (MqSException*) MqSysCalloc(MQ_ERROR_PANIC, 1, sizeof(*ex)); \
    ex->num = MqErrorGetNum(context); \
    ex->code = MqErrorGetCodeI(context); \
    ex->text = MqSysStrDup(MQ_ERROR_PRINT, MqErrorGetText(context)); \
    sv_setref_pv(errsv, "Net::PerlMsgque::MqSException", (void*)ex); \
    MqErrorReset(context); \
    croak(NULL); \
  }

#define printSV(x,sv) MqLogV(x,__func__,0,"\n ---> interp<%p> thread<%ld>, ptr<%p>, refcnt<%d>, type<%s>, val<%s>\n", \
    PERL_GET_CONTEXT, pthread_self(), sv, SvREFCNT((SV*)sv), sv_reftype((SV*)sv,1), SvPV_nolen((SV*)sv));fflush(stdout);

#define printID(x) MqLogV(x,__func__,0,"PERL_GET_CONTEXT<%p> context->threadData<%p>\n", PERL_GET_CONTEXT, x?x->threadData:NULL);

#define MQ_CONTEXT_S context

typedef MQ_CST MQ_NST;

struct MqSException {
  int num;
  int code;
  char * text;
};

struct PerlContextS {
  struct MqS mqctx;
  HV * data;
};

#define PERL_DATA (((struct PerlContextS*)context)->data)

typedef struct MqS MqS;
typedef struct MqS MqSelf;
typedef struct MqSException MqSException;
typedef struct MqBufferS MqBufferS;
typedef struct MqBufferS MqBufferSelf;
typedef struct MqDumpS MqDumpS;
typedef struct MqDumpS MqDumpSelf;
typedef struct MqFactoryS MqFactoryS;
typedef struct MqFactoryS MqFactorySelf;

static enum MqErrorE DummyOK (
  struct MqS * const context
)
{
  return MQ_OK;
}

static mq_inline MqS* get_MqS_NO_ERROR(SV* sv)
{
  MqS *var = NULL;
  if (SvROK(sv)) {
    sv = (SV*)SvRV(sv);
    if (SvIOK(sv)) {
      var = INT2PTR(MqS*,SvIV(sv));
      if (var != NULL && var->signature != MQ_MqS_SIGNATURE) {
	var = NULL;
      }
    }
  }
  return var;
}

static MqS* get_MqS(pTHX_ SV* sv)
{
  MqS *var = NULL;
  if (SvROK(sv)) {
    sv = (SV*)SvRV(sv);
    if (SvIOK(sv)) {
      var = INT2PTR(MqS*,SvIV(sv));
      if (var->signature != MQ_MqS_SIGNATURE) {
	var = NULL;
      }
    }
  }
  if (var == NULL) {
    Perl_croak(aTHX_ "object is not of type Net::PerlMsgque::MqS");
  }
  return var;
}

static mq_inline MqSelf* get_MqSelf(pTHX_ SV* sv)
{
  return INT2PTR(MqSelf*,SvIV((SV*)SvRV(sv)));
}

static MqBufferS* get_MqBufferS(pTHX_ SV* sv)
{
  MqBufferS *var = NULL;
  if (SvROK(sv)) {
    sv = (SV*)SvRV(sv);
    if (SvIOK(sv)) {
      var = INT2PTR(MqBufferS*,SvIV(sv));
      if (var->signature != MQ_MqBufferS_SIGNATURE) {
	var = NULL;
      }
    }
  }
  if (var == NULL) {
    Perl_croak(aTHX_ "object is not of type Net::PerlMsgque::MqBufferS");
  }
  return var;
}

static mq_inline MqBufferSelf* get_MqBufferSelf(pTHX_ SV* sv)
{
  return INT2PTR(MqBufferSelf*,SvIV((SV*)SvRV(sv)));
}

static MqFactoryS* get_MqFactoryS(pTHX_ SV* sv)
{
  MqFactoryS *var = NULL;
  if (SvROK(sv)) {
    sv = (SV*)SvRV(sv);
    if (SvIOK(sv)) {
      var = INT2PTR(MqFactoryS*,SvIV(sv));
      if (var->signature != MQ_MqFactoryS_SIGNATURE) {
	var = NULL;
      }
    }
  }
  if (var == NULL) {
    Perl_croak(aTHX_ "object is not of type Net::PerlMsgque::MqFactoryS");
  }
  return var;
}

static mq_inline MqFactorySelf* get_MqFactorySelf(pTHX_ SV* sv)
{
  return INT2PTR(MqFactorySelf*,SvIV((SV*)SvRV(sv)));
}

static MqDumpS* get_MqDumpS(pTHX_ SV* sv)
{
  MQ_INT *var = NULL;
  if (SvROK(sv)) {
    sv = (SV*)SvRV(sv);
    if (SvIOK(sv)) {
      IV tiv = SvIV(sv);
      var = INT2PTR(MQ_INT*,tiv);
      if (*var != MQ_MqDumpS_SIGNATURE) {
	var = NULL;
      }
    }
  }
  if (var == NULL) {
    Perl_croak(aTHX_ "object is not of type Net::PerlMsgque::MqDumpS");
  }
  return (MqDumpS*) var;
}

static mq_inline MqDumpSelf* get_MqDumpSelf(pTHX_ SV* sv)
{
  return INT2PTR(MqDumpSelf*,SvIV((SV*)SvRV(sv)));
}

static enum MqErrorE
ProcError (pTHX_ MqS * context, SV* err)
{
  if (SvTRUE(err)) {
    if (sv_isobject(err) && sv_derived_from(err, "Net::PerlMsgque::MqSException")) {
      MqSException *ex = INT2PTR(MqSException*, SvIV(SvRV(err)));
      MqErrorSet (context, ex->num, ex->code, ex->text, NULL);
      MqSysFree (ex->text);
      MqSysFree(ex);
    } else {
      MqErrorC (context, __func__, -1, SvPV_nolen(err));
    }
  }
  return MqErrorGetCodeI(context);
}

static enum MqErrorE
ProcCall (
  struct MqS * const context,
  MQ_PTR const data
)
{
  dSP;
  SV * method = (SV*) data;

  ENTER;
  SAVETMPS;

  if (SvROK((SV*)context->self)) {
    PUSHMARK(SP);
    XPUSHs((SV*)context->self);
    PUTBACK;
    call_sv (method, G_SCALAR|G_DISCARD|G_EVAL);
    ProcError (aTHX_ context, ERRSV);
  }

  FREETMPS;
  LEAVE;

  return MqErrorGetCodeI(context);
}

static void
ProcFree (
  struct MqS const * const context,
  MQ_PTR *dataP
)
{
  SvREFCNT_dec(dataP);
  *dataP = NULL;
}

static void
ProcCopy (
  struct MqS * const context,
  MQ_PTR *dataP
)
{
  *dataP = newSVsv(*dataP);
  //return MqErrorC(context,__func__,-1,"perl requires that a 'callback' have to be defined in the object constructor");
}

static void
FactoryFree (
  MQ_PTR *dataP
)
{
  SvREFCNT_dec(dataP);
  *dataP = NULL;
}

static void
FactoryCopy (
  MQ_PTR *dataP
)
{
  *dataP = newSVsv(*dataP);
}


static void
ProcessExit (
  MQ_INT num
)
{
  my_exit(num);
}

static void
ThreadExit (
  MQ_INT num
)
{
}

static enum MqErrorE FactoryCreate (
  struct MqS * const	  tmpl,
  enum MqFactoryE	  create,
  struct MqFactoryS *	  item,
  struct MqS **		  contextP
) {
  struct MqS * mqctx = NULL;
  enum MqErrorE ret = MQ_OK;
  int count;

#ifdef MQ_HAS_THREAD
  if (create == MQ_FACTORY_NEW_THREAD) {
    perl_clone ((PerlInterpreter*)tmpl->threadData, CLONEf_CLONE_HOST);
  }
#endif
  {
    dSP;
    *contextP = NULL;

    ENTER;
    SAVETMPS;

    PUSHMARK(SP);

    XPUSHs((SV*)item->Create.data);
    if (create != MQ_FACTORY_NEW_INIT) {
      XPUSHs((SV*)tmpl->self);
    }
    PUTBACK;

    count = call_method ("new", G_SCALAR|G_EVAL);

    SPAGAIN;
    if (SvTRUE(ERRSV)) {
      goto error1;
    } else if (count != 1) {
      if (create != MQ_FACTORY_NEW_INIT) {
	MqErrorC(tmpl, __func__, -1, "factory return more than one value!");
      } else {
	croak("factory return more than one value!");
      }
      return MQ_ERROR;  
    }

    mqctx = get_MqS_NO_ERROR(POPs);
    if (mqctx == NULL) goto error2;

    // check for MQ error
    MqErrorCheck(MqErrorGetCode(mqctx));

    if (create != MQ_FACTORY_NEW_INIT) {
      MqSetupDup (mqctx, tmpl);
    }

    PUTBACK;
    FREETMPS;
    LEAVE;

    *contextP = mqctx;
    return ret;

  error:
    *contextP = NULL;
    if (create != MQ_FACTORY_NEW_INIT) {
      MqErrorCopy (tmpl, mqctx);
      MqContextDelete (&mqctx);
      return MqErrorStack(tmpl);
    } else {
      return MQ_ERROR;
    }

  error1:
    *contextP = NULL;
    if (create != MQ_FACTORY_NEW_INIT) {
      return ProcError (aTHX_ tmpl, ERRSV);
    } else {
      croak(SvPV_nolen(ERRSV));
      return MQ_ERROR;
    }

  error2:
    *contextP = NULL;
    if (create != MQ_FACTORY_NEW_INIT) {
      return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
    } else {
      croak("Factory return no MqS type");
      return MQ_ERROR;
    }
  }
}

static void FactoryDelete (
  struct MqS * context,
  MQ_BOL doFactoryCleanup,
  struct MqFactoryS * const item
) { 
  enum MqStatusIsE statusIs = context->statusIs;
  PerlInterpreter *itp = context->threadData;
  if (statusIs & MQ_STATUS_IS_THREAD) {
    perl_destruct (itp);
    perl_free (itp);
  } else {
    sv_unref((SV*)context->self);
    MqContextFree(context);
  }
}

MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque PREFIX = Mq

BOOT:
MqLal.SysIgnorSIGCHLD = DummyOK;
MqLal.SysAllowSIGCHLD = DummyOK;
// default factory
MqFactoryDefault(MQ_ERROR_PRINT, "perlmsgque", FactoryCreate, newSVpv("Net::PerlMsgque::MqS",0), 
    FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL);

void
MqCleanup()

void
Init(...)
  CODE:
    int i;
    struct MqBufferLS * args = MqInitArg0(NULL,NULL);
    for (i=0; i<items; i++) {
      MqBufferLAppendC (args, (char *)SvPV_nolen(ST(i)));
    }

void
Resolve(MQ_CST ident)
  PREINIT:
    MqS ** ctxP;
  PPCODE:
    for (ctxP=MqResolve(ident); *ctxP != NULL; ctxP++) {
      XPUSHs((SV*)ctxP[0]->self);
    }

void
PrintID(MqS *context)
  CODE:
    printID(context)

BOOT:
  MqLal.SysFork = (MqSysForkF) my_fork;

MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque::MqS

void
new(SV *MqS_class, ...)
  PREINIT:
    MqS * ctx  = NULL;
    MqS * tmpl = NULL;
  PPCODE:
    if (items == 2) {
      tmpl = get_MqS(aTHX_ ST(1));
    } else if (items != 1) {
      croak_xs_usage(cv, "?tmpl?");
      XSRETURN(0);
    }
    if (!SvROK(MqS_class)) {
      // called by a "class"
      ctx = (MqS*) MqContextCreate(sizeof(struct PerlContextS), tmpl);
      ST(0) = sv_newmortal();
      sv_setref_pv(ST(0), SvPV_nolen(MqS_class), (void*)ctx);
      ctx->threadData = PERL_GET_CONTEXT;
#if defined(MQ_IS_WIN32)
      // no way to perl attach to a thread on windows.
      MqConfigSetIgnoreThread(ctx,MQ_YES);
#endif
      MqConfigSetSelf(ctx, SvREFCNT_inc(ST(0)));
      MqConfigSetSetup(ctx, MqLinkDefault, NULL, MqLinkDefault, NULL, ProcessExit, ThreadExit);
    } else {
      // called by a "object"
      ctx = get_MqS (aTHX_ MqS_class);
      ST(0) = (SV*) ctx->self;
      MqConfigReset (ctx);
    }
    XSRETURN(1);

void
DESTROY(SV *sv)
  PREINIT:
    MqS *context;
  CODE:
    context = get_MqS_NO_ERROR(sv);
    // well "perl_clone" clone everything including the "perl" objects
    // from the orignal interpreter. These objects get an "DESTROY" if
    // "perl_clone" interpreter is destroyed with "perl_destruct"
    // -> this is not good because the according struct MqS* is freed even
    //    if the object in the original interpreter is still active
    // -> do NOT delete the struct MqS* if the object was !not! created in
    //	  current interpreter
    if (context != NULL && SvREFCNT(sv) == 1 && PERL_GET_CONTEXT == (PerlInterpreter*)context->threadData) {
      HV* hash = PERL_DATA;
      // set context to NULL
      if (context->self != NULL) {
	sv_setref_pv((SV*)context->self, "", (void*)NULL);
      }
      // the "Factory" is useless -> delete
      context->setup.factory = NULL;
      // delete the context
      MqContextDelete(&context);
      // free the data
      if (hash) hv_undef (hash);
    }


MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque::MqFactoryS  PREFIX = MqFactory

MqFactoryS*
MqFactoryAdd(...)
  PREINIT:
    MQ_CST ident = "";
    MQ_PTR class = NULL;
  CODE:
    if (items < 1 || items > 2) {
      croak_xs_usage(cv, "?ident?, class");
    } else if (items == 1) {
      ident = SvPV_nolen(ST(0));
      class = newSVsv(ST(0));
    } else {  // items == 2
      ident = SvPV_nolen(ST(0));
      class = newSVsv(ST(1));
    }
    RETVAL = MqFactoryAdd(MQ_ERROR_PRINT, ident, 
	      FactoryCreate, class, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL);
  OUTPUT:
    RETVAL

MqFactoryS*
MqFactoryDefault(...)
  PREINIT:
    MQ_CST ident = "";
    MQ_PTR class = NULL;
  CODE:
    if (items < 1 || items > 2) {
      croak_xs_usage(cv, "ident, ?class?");
    } else if (items == 1) {
      ident = SvPV_nolen(ST(0));
      class = newSVpv("Net::PerlMsgque::MqS",0);
    } else {  // items == 2
      ident = SvPV_nolen(ST(0));
      class = newSVsv(ST(1));
    }
    RETVAL = MqFactoryDefault(MQ_ERROR_PRINT, ident, 
	      FactoryCreate, class, FactoryFree, FactoryCopy, FactoryDelete, NULL, NULL, NULL);
  OUTPUT:
    RETVAL

MQ_CST
MqFactoryDefaultIdent()

MqFactoryS*
MqFactoryGet(MQ_CST ident = NULL)

MqFactoryS*
MqFactoryGetCalled(MQ_CST ident = NULL)

void
MqFactoryNew(MqFactorySelf* factory)
  PREINIT:
    struct MqS * ctx = NULL;
  PPCODE:
    ctx = MqFactoryNew(MQ_ERROR_PRINT, NULL, factory);
    if (ctx == NULL) {
      croak("MqFactoryS exception");
      XSRETURN(0);
    } else {
      ST(0) = (SV*)ctx->self;
      XSRETURN(1);
    }

MqFactoryS*
MqFactoryCopy(MqFactorySelf* factory, MQ_CST ident)


MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque::MqS  PREFIX = Mq

void
MqErrorSet(MqSelf* context, SV* err)
  CODE:
    ProcError (aTHX_ context, err);

void
MqErrorSetCONTINUE(MqSelf* context)

void
MqErrorSetEXIT(MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck(MqErrorSetEXIT(context))

mq_bool
MqErrorIsEXIT(MqSelf* context)

void
MqErrorPrint(MqSelf* context)

void
MqErrorReset(MqSelf* context)

void
MqErrorRaise(MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck(MqErrorGetCodeI(context))

void
MqErrorC (MqSelf* context, MQ_CST prefix, MQ_INT error_number, MQ_CST error_text)

MQ_INT
MqErrorGetNum (MqSelf* context)

MQ_CST
MqErrorGetText (MqSelf* context)

void
MqProcessEvent(MqSelf* context, ...)
  PREINIT:
    MQ_TIME_T timeout = MQ_TIMEOUT_DEFAULT;
    enum MqWaitOnEventE wait = MQ_WAIT_NO;
  CODE:
    if (items == 3) {
      timeout = SvIV(ST(1));
      wait = SvIV(ST(2));
    } else if (items == 2) {
      wait = SvIV(ST(1));
    } else if (items == 1) {
      // do nothing
    } else {
      croak_xs_usage(cv, "?timeout?, ?wait?");
      XSRETURN(0);
    }
    ErrorMqToPerlWithCheck (MqProcessEvent (context, timeout, wait));

void
LinkGetParent(MqSelf* context)
  PREINIT:
    MqS* parent;
  PPCODE:
    parent = (MqS*) MqLinkGetParent(context);
    ST(0) = (parent != NULL? (SV*)parent->self : &PL_sv_undef);
    XSRETURN(1);

MQ_CST
MqLinkGetTargetIdent (MqSelf* context)

mq_bool
MqLinkIsConnected (MqSelf* context)

mq_bool
MqLinkIsParent (MqSelf* context)

int
MqLinkGetCtxId (MqSelf* context)

void
MqLinkCreate(MqS * context, ...)
  PREINIT:
    struct MqBufferLS * args = NULL;
  CODE:
    if (items > 1) {
      MQ_CST str;
      int i;
      args = MqBufferLCreate (items);
      for (i=1; i<items; i++) {
	str = (char *)SvPV_nolen(ST(i));
	if (i==1 && (str[0] == '-' || str[0] == MQ_ALFA)) {
	  // "MqInitGet" set in lib/Net/PerlMsgque.pm -> Init
	  MqBufferLAppendC (args, MqInitGetArg0()->data[2]->cur.C);
	}
	MqBufferLAppendC (args, str);
      }
    }
    ErrorMqToPerlWithCheck (MqLinkCreate(context, &args));

void
MqLinkCreateChild(MqS *context, MqS *parent, ...)
  PREINIT:
    struct MqBufferLS * args = NULL;
  CODE:
    if (items > 2) {
      MQ_CST str;
      int i;
      args = MqBufferLCreate (items-1);
      for (i=2; i<items; i++) {
	str = (char *)SvPV_nolen(ST(i));
	if (i==2 && (str[0] == '-' || str[0] == MQ_ALFA)) {
	  // "MqInitGet" set in lib/Net/PerlMsgque.pm -> Init
	  MqBufferLAppendC (args, MqInitGetArg0()->data[2]->cur.C);
	}
	MqBufferLAppendC (args, (char *)SvPV_nolen(ST(i)));
      }
    }
    ErrorMqToPerlWithCheck (MqLinkCreateChild(context, parent, &args));

void
MqLinkDelete(MqS *context)

void
MqLinkConnect (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqLinkConnect (context));

void
MqExit(MqS * context)

void
MqDelete (MqSelf* context)
  CODE:
    MqContextFree (context);

void
MqLogC(MqS * context, MQ_CST prefix, int level, MQ_CST text)

void
MqConfigSetBuffersize (MqSelf* context, int buffersize)

int
MqConfigGetBuffersize (MqSelf* context)

void
MqConfigSetDebug (MqSelf* context, int debug)

int
MqConfigGetDebug (MqSelf* context)

void
MqConfigSetTimeout (MqSelf* context, int timeout)

MQ_TIME_T
MqConfigGetTimeout (MqSelf* context)

void
MqConfigSetName (MqSelf* context, MQ_NST name)

MQ_NST
MqConfigGetName (MqSelf* context)

void
MqConfigSetSrvName (MqSelf* context, MQ_NST srvname)

MQ_NST
MqConfigGetSrvName (MqSelf* context)

void
MqConfigSetStorage (MqSelf* context, MQ_NST storageFile)

MQ_NST
MqConfigGetStorage (MqSelf* context)

void
MqConfigSetIoUdsFile (MqSelf* context, MQ_NST udsfile)
  CODE:
    ErrorMqToPerlWithCheck(MqConfigSetIoUdsFile(context, udsfile))

MQ_NST
MqConfigGetIoUdsFile (MqSelf* context)

void
MqConfigSetIoTcp (MqSelf* context, MQ_NST host, MQ_NST port, MQ_NST myhost, MQ_NST myport)
  CODE:
    ErrorMqToPerlWithCheck(MqConfigSetIoTcp(context, host, port, myhost, myport))

MQ_NST
MqConfigGetIoTcpHost (MqSelf* context)

MQ_NST
MqConfigGetIoTcpPort (MqSelf* context)

MQ_NST
MqConfigGetIoTcpMyHost (MqSelf* context)

MQ_NST
MqConfigGetIoTcpMyPort (MqSelf* context)

void
MqConfigSetIoPipeSocket (MqSelf* context, MQ_INT socket)
  CODE:
    ErrorMqToPerlWithCheck(MqConfigSetIoPipeSocket(context, socket))

MQ_INT
MqConfigGetIoPipeSocket (MqSelf* context)

void
MqConfigSetStartAs (MqSelf* context, MQ_INT startAs)

MQ_INT
MqConfigGetStartAs (MqSelf* context)

MQ_INT
MqConfigGetStatusIs (MqSelf* context)

void
MqConfigSetDaemon (MqSelf* context, MQ_CST pidfile)

void
MqConfigSetIsSilent (MqSelf* context, MQ_BOL isSilent)

mq_bool
MqConfigGetIsSilent (MqSelf* context)

void
MqConfigSetIsString (MqSelf* context, MQ_BOL isString)

void
MqConfigSetIgnoreExit (MqSelf* context, MQ_BOL ignoreExit)

mq_bool
MqConfigGetIsString (MqSelf* context)

void
MqConfigSetIsServer (MqSelf* context, MQ_BOL isServer)

mq_bool
MqConfigGetIsServer (MqSelf* context)

void
MqConfigSetServerSetup (MqSelf* context, SV* setupF)
  CODE:
    MqConfigSetServerSetup (context, ProcCall, (MQ_PTR) newSVsv(setupF), ProcFree, ProcCopy);

void
MqConfigSetServerCleanup (MqSelf* context, SV* cleanupF)
  CODE:
    MqConfigSetServerCleanup (context, ProcCall, (MQ_PTR) newSVsv(cleanupF), ProcFree, ProcCopy);

void
MqConfigSetBgError (MqSelf* context, SV* bgerrorF)
  CODE:
    MqConfigSetBgError (context, ProcCall, (MQ_PTR) newSVsv(bgerrorF), ProcFree, ProcCopy);

void
MqConfigSetEvent (MqSelf* context, SV* eventF)
  CODE:
    MqConfigSetEvent (context, ProcCall, (MQ_PTR) newSVsv(eventF), ProcFree, ProcCopy);



MqFactoryS*
MqFactoryCtxGet (MqSelf* context)

void
MqFactoryCtxSet (MqSelf* context, MqFactoryS* factory)
  CODE:
    ErrorMqToPerlWithCheck (MqFactoryCtxSet (context, factory));

MQ_CST
MqFactoryCtxIdentGet (MqSelf* context)

void
MqFactoryCtxIdentSet (MqSelf* context, MQ_NST ident)
  CODE:
    ErrorMqToPerlWithCheck (MqFactoryCtxIdentSet (context, ident));


void
MqSendSTART (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendSTART (context));

void
MqSendEND_AND_WAIT (MqSelf* context, MQ_CST token, MQ_TIME_T timeout = MQ_TIMEOUT_USER)
  CODE:
    ErrorMqToPerlWithCheck (MqSendEND_AND_WAIT(context, token, timeout));

void
MqSendEND_AND_CALLBACK (MqSelf* context, MQ_CST token, SV* callbackF)
  CODE:
    ErrorMqToPerlWithCheck (MqSendEND_AND_CALLBACK(context, token, ProcCall, (MQ_PTR) newSVsv(callbackF), ProcFree));

void
MqSendEND (MqSelf* context, MQ_CST token)
  CODE:
    ErrorMqToPerlWithCheck (MqSendEND(context, token));

void
MqSendRETURN (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendRETURN (context));

void
MqSendERROR (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendERROR (context));

void
MqSendO (MqSelf* context, MQ_BOL boolean_val)
  CODE:
    ErrorMqToPerlWithCheck (MqSendO (context, boolean_val));

MQ_BOL
MqReadO (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadO (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendY (MqSelf* context, MQ_BYT byte)
  CODE:
    ErrorMqToPerlWithCheck (MqSendY (context, byte));

MQ_BYT
MqReadY (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadY (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendS (MqSelf* context, MQ_SRT short_int)
  CODE:
    ErrorMqToPerlWithCheck (MqSendS (context, short_int));

MQ_SRT
MqReadS (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadS (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendI (MqSelf* context, MQ_INT integer)
  CODE:
    ErrorMqToPerlWithCheck (MqSendI (context, integer));

MQ_INT
MqReadI (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadI (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendW (MqSelf* context, MQ_WID wide_int)
  CODE:
    ErrorMqToPerlWithCheck (MqSendW (context, wide_int));

MQ_WID
MqReadW (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadW (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendF (MqSelf* context, MQ_FLT float_val)
  CODE:
    ErrorMqToPerlWithCheck (MqSendF (context, float_val));

MQ_FLT
MqReadF (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadF (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendD (MqSelf* context, MQ_DBL double_val)
  CODE:
    ErrorMqToPerlWithCheck (MqSendD (context, double_val));

MQ_DBL
MqReadD (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadD (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendC (MqSelf* context, MQ_CST string)
  CODE:
    ErrorMqToPerlWithCheck (MqSendC (context, string));

MQ_CST
MqReadC (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadC (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqSendB (MqSelf* context, SV* binary)
  INIT:
    MQ_CBI bin;
    STRLEN len;
  CODE:
    bin = (MQ_BIN)SvPVbyte (binary, len);
    ErrorMqToPerlWithCheck (MqSendB (context, bin, (MQ_SIZE)len));

SV*
MqReadB (MqSelf* context)
  INIT:
    MQ_BIN bin;
    MQ_SIZE len;
  CODE:
    ErrorMqToPerlWithCheck (MqReadB (context, &bin, &len));
    RETVAL = newSVpvn((MQ_CST)bin, len);
  OUTPUT:
    RETVAL

void
MqSendN (MqSelf* context, SV* binary)
  INIT:
    MQ_CBI bin;
    STRLEN len;
  CODE:
    bin = (MQ_BIN)SvPVbyte (binary, len);
    ErrorMqToPerlWithCheck (MqSendN (context, bin, (MQ_SIZE)len));

SV*
MqReadN (MqSelf* context)
  INIT:
    MQ_CBI bin;
    MQ_SIZE len;
  CODE:
    ErrorMqToPerlWithCheck (MqReadN (context, &bin, &len));
    RETVAL = newSVpvn((MQ_CST)bin, len);
  OUTPUT:
    RETVAL

void
MqSendU (MqSelf* context, MqBufferS *buffer)
  CODE:
    ErrorMqToPerlWithCheck (MqSendU (context, buffer));

MqBufferS*
MqReadU (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadU (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqReadProxy (MqSelf* context, MqS* target)
  CODE:
    ErrorMqToPerlWithCheck (MqReadProxy (context, target));

MqDumpS*
MqReadDUMP (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadDUMP (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqReadLOAD (MqSelf* context, MqDumpS* dump)
  CODE:
    ErrorMqToPerlWithCheck (MqReadLOAD (context, dump));

void
MqReadForward (MqSelf* context, MqS* ftr, MqDumpS* dump = NULL)
  CODE:
    ErrorMqToPerlWithCheck (MqReadForward (context, ftr, dump));

MQ_NST
MqServiceGetToken (MqSelf* context)

void
MqServiceGetFilter(MqSelf* context, MQ_SIZE id = 0)
  PREINIT:
    struct MqS * filter = NULL;
  PPCODE:
    ErrorMqToPerlWithCheck (MqServiceGetFilter (context, id, &filter));
    ST(0) = (SV*)filter->self;
    XSRETURN(1);

mq_bool
MqServiceIsTransaction (MqSelf* context)
    
void
MqServiceProxy (MqSelf* context, MQ_CST token, MQ_SIZE id = 0)
  CODE:
    ErrorMqToPerlWithCheck (MqServiceProxy (context, token, id));
    
void
MqServiceStorage (MqSelf* context, MQ_CST token)
  CODE:
    ErrorMqToPerlWithCheck (MqServiceStorage (context, token));
    
void
MqServiceCreate (MqSelf* context, MQ_CST token, SV* serviceF)
  CODE:
    ErrorMqToPerlWithCheck (
      MqServiceCreate (context, token, ProcCall, (MQ_PTR) newSVsv(serviceF), ProcFree)
    );

# -------------------------------------------------------------------------------------
    
void
MqStorageOpen (MqSelf* context, MQ_CST storageFile)
  CODE:
    ErrorMqToPerlWithCheck (MqStorageOpen (context, storageFile));
    
void
MqStorageClose (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqStorageClose (context));
    
MQ_TRA
MqStorageInsert (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqStorageInsert (context, &RETVAL));
  OUTPUT:
    RETVAL

MQ_TRA
MqStorageSelect (MqSelf* context, MQ_TRA transLId = 0)
  CODE:
    RETVAL = transLId;
    ErrorMqToPerlWithCheck (MqStorageSelect (context, &RETVAL));
  OUTPUT:
    RETVAL

void
MqStorageDelete (MqSelf* context, MQ_TRA transLId)
  CODE:
    ErrorMqToPerlWithCheck (MqStorageDelete (context, transLId));
    
MQ_TRA
MqStorageCount (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqStorageCount (context, &RETVAL));
  OUTPUT:
    RETVAL

# -------------------------------------------------------------------------------------

SV*
DictSet (MqSelf* context, MQ_CST key, SV* data)
  PREINIT:
    SV** svP = NULL;
  CODE:
    {
      HV* hash = PERL_DATA;
      if (hash == NULL) {
	hash = PERL_DATA = newHV();
      }
      svP = hv_store(hash, key, (I32) strlen(key), SvREFCNT_inc(data), 0);
    }
    if (svP == NULL) {
      SvREFCNT_dec(data);
      croak ("unable to save data for key '%s'", key);
    }
    RETVAL = SvREFCNT_inc(*svP);
  OUTPUT:
    RETVAL

SV*
DictGet (MqSelf* context, MQ_CST key)
  PREINIT:
    SV** svP = NULL;
  CODE:
    if (PERL_DATA != NULL) {
      svP = hv_fetch(PERL_DATA, key, (I32) strlen(key), 0);
    }
    RETVAL = SvREFCNT_inc(svP ? *svP : &PL_sv_undef);
  OUTPUT:
    RETVAL

SV*
DictUnset (MqSelf* context, MQ_CST key)
  PREINIT:
    SV* sv = NULL;
  CODE:
    if (PERL_DATA != NULL) {
      sv = hv_delete (PERL_DATA, key, (I32) strlen(key),0);
    }
    RETVAL = SvREFCNT_inc (sv ? sv : &PL_sv_undef);
  OUTPUT:
    RETVAL

bool
DictExists (MqSelf* context, MQ_CST key)
  PREINIT:
    bool exists = (bool) FALSE;
  CODE:
    if (PERL_DATA != NULL) {
      exists = hv_exists (PERL_DATA, key, (I32) strlen(key));
    }
    RETVAL = exists;
  OUTPUT:
    RETVAL

mq_bool
MqReadItemExists(MqSelf* context)

MQ_SIZE
MqReadGetNumItems(MqSelf* context)

void
MqReadUndo(MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadUndo (context));

void
MqSendL_START (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendL_START(context));

void
MqSendL_END (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendL_END(context));

void
MqSendT_START (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqSendT_START(context));

void
MqSendT_END (MqSelf* context, MQ_CST ident)
  CODE:
    ErrorMqToPerlWithCheck (MqSendT_END(context, ident));

void
MqReadL_START (MqSelf* context, MqBufferS* buffer = NULL)
  CODE:
    ErrorMqToPerlWithCheck (MqReadL_START (context, buffer)); 

void
MqReadL_END (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadL_END(context));

void
MqReadT_START (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadT_START (context)); 

void
MqReadT_END (MqSelf* context)
  CODE:
    ErrorMqToPerlWithCheck (MqReadT_END(context));

void
MqSysUSleep (MqSelf* context, unsigned int usec)
  CODE:
    ErrorMqToPerlWithCheck (MqSysUSleep(context, usec));

void
MqSlaveWorker (MqSelf* context, MQ_SIZE id, ...)
  PREINIT:
    struct MqBufferLS * args = NULL;
  CODE:
    if (items > 2) {
      int i;
      args = MqBufferLCreate (items-2);
      for (i=2; i<items; i++) {
	MqBufferLAppendC (args, (char *)SvPV_nolen(ST(i)));
      }
    }
    ErrorMqToPerlWithCheck (MqSlaveWorker(context, id, &args));

void
MqSlaveCreate (MqSelf* context, MQ_SIZE id, MqS* slave)
  CODE:
    ErrorMqToPerlWithCheck (MqSlaveCreate(context, id, slave));

void
MqSlaveDelete (MqSelf* context, MQ_SIZE id)
  CODE:
    ErrorMqToPerlWithCheck (MqSlaveDelete(context, id));

void
MqSlaveGet (MqSelf* context, MQ_SIZE id)
  PREINIT:
    MqS* slave;
  PPCODE:
    slave = (MqS*) MqSlaveGet(context,id);
    ST(0) = (slave ? (SV*)slave->self : &PL_sv_undef);
    XSRETURN(1);

void
SlaveGetMaster(MqSelf* context)
  PREINIT:
    MqS* master;
  PPCODE:
    master = (MqS*) MqSlaveGetMaster(context);
    ST(0) = (master ? (SV*)master->self : &PL_sv_undef);
    XSRETURN(1);

mq_bool
MqSlaveIs (MqSelf* context)

void
MqLog (MqSelf* context, MQ_CST prefix, MQ_INT level, MQ_CST str)
  CODE:
    MqLogC(context,prefix,level,str);


MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque::MqDumpS

void
DESTROY(MqDumpSelf *dump)
  CODE:
    MqDumpDelete(&dump);

MQ_INT
Size (MqDumpSelf *dump)
  CODE:
    RETVAL = MqDumpSize(dump);
  OUTPUT:
    RETVAL

MODULE = Net::PerlMsgque PACKAGE = Net::PerlMsgque::MqBufferS

MqBufferS*
Dup (MqBufferSelf *buffer)
  CODE:
    RETVAL = MqBufferDup(buffer);
  OUTPUT:
    RETVAL

void
Delete (MqBufferSelf *buffer)
  CODE:
    MqBufferDelete(&buffer);

char
GetType (MqBufferSelf *buffer)
  CODE:
    RETVAL = MqBufferGetType(buffer);
  OUTPUT:
    RETVAL

MQ_BYT
GetY (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetY(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_BOL
GetO (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetO(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_SRT
GetS (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetS(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_INT
GetI (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetI(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_WID
GetW (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetW(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_FLT
GetF (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetF(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_DBL
GetD (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetD(buffer, &RETVAL));
  OUTPUT:
    RETVAL

MQ_CST
GetC (MqBufferSelf *buffer)
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetC(buffer, &RETVAL));
  OUTPUT:
    RETVAL

SV*
GetB (MqBufferSelf *buffer)
  INIT:
    MQ_BIN bin;
    MQ_SIZE len;
  CODE:
    ErrorBufferToPerlWithCheck(MqBufferGetB(buffer, &bin, &len));
    RETVAL = newSVpvn((MQ_CST)bin, len);
  OUTPUT:
    RETVAL

