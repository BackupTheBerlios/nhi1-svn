/**
 *  \file       theLink/ccmsgque/ccmsgque.h
 *  \brief      \$Id$
 *  
 *  (C) 2004 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#ifndef MQ_CCMSGQUE_H
#define MQ_CCMSGQUE_H

#include <exception>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <typeinfo>

#if !defined(MQ_COMPILE_AS_CC)
# define MQ_COMPILE_AS_CC
#endif
#include "msgque.h"

/// \defgroup Mq_CC_API Mq_CC_API
/// \copydoc Mq_C_API
/// \{

/// \brief The CcMsgque Libraray
namespace ccmsgque {

  using namespace std;
  using namespace libmsgque;

  class MqC;
  class MqCException;

  /// \brief Callback for a Service
  class IService {
    public:
      virtual void Service (MqC * const ctx) = 0;
  };

  /// \api #MqConfigSetServerSetup
  class IServerSetup {
    public:
      virtual void ServerSetup () = 0;
  };

  /// \api #MqConfigSetServerCleanup
  class IServerCleanup {
    public:
      virtual void ServerCleanup () = 0;
  };

  /// \api #MqConfigSetBgError
  class IBgError {
    public:
      virtual void BgError () = 0;
  };

  /// \api #MqConfigSetEvent
  class IEvent {
    public:
      virtual void Event () = 0;
  };

  /// \ingroup Mq_Error_CC_API
  /// \api #MqErrorS
  class MqCException : public exception
  {
    private:
      MQ_STR p_message;
      MQ_INT p_num;
      enum MqErrorE p_code;

    public:
      MQ_EXTERN MqCException(struct MqS *const context);
      MqCException(int mynum, enum MqErrorE mycode, MQ_CST mytxt) : 
	p_message (mq_strdup(mytxt)), p_num (mynum), p_code (mycode) { }
      MqCException(int mynum, enum MqErrorE mycode, MQ_STR mytxt) : 
	p_message (mytxt), p_num (mynum), p_code (mycode) { }
      MQ_EXTERN MqCException(int mynum, enum MqErrorE mycode, MQ_CST myfunc, 
		    MQ_CST myfile, const int myline, MQ_CST mytxt);
      MqCException(const MqCException& e) : 
	p_message (mq_strdup(e.p_message)), p_num (e.p_num), p_code (e.p_code) { }
      virtual ~MqCException() throw() {
	MqSysFree (p_message);
      }
      virtual MQ_CST what() const throw() {
	return p_message;
      }
      virtual MQ_INT num() const throw() {
	return p_num;
      }
      virtual enum MqErrorE code() const throw() {
	return p_code;
      }
  };
 
  /// \ingroup Mq_Buffer_CC_API
  /// \api #MqBufferS
  class MqBufferC {
    /// \defgroup Mq_Buffer_CC_API Mq_Buffer_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Buffer_C_API
    /// \details \copydetails Mq_Buffer_C_API
    /// \{
    private:
      struct MqBufferS * hdl;

      inline void ErrorCheck(enum MqErrorE err) const throw(MqCException) {
	if (MqErrorCheckI(err)) throw MqCException(hdl->context);
      }

    public:

      MqBufferC (struct MqBufferS *bufP) {
	hdl = bufP;
      }

      /// \api #MqBufferS::type
      inline enum MqTypeE GetType () const {
	return hdl->type;
      }

      /// \api #MqBufferGetY
      inline MQ_BYT GetY () const throw(MqCException) {
	MQ_BYT out;
	ErrorCheck (MqBufferGetY (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetO
      inline MQ_BOL GetO () const throw(MqCException) {
	MQ_BOL out;
	ErrorCheck (MqBufferGetO (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetS
      inline MQ_SRT GetS () const throw(MqCException) {
	MQ_SRT out;
	ErrorCheck (MqBufferGetS (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetI
      inline MQ_INT GetI () const throw(MqCException) {
	MQ_INT out;
	ErrorCheck (MqBufferGetI (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetF
      inline MQ_FLT GetF () const throw(MqCException) {
	MQ_FLT out;
	ErrorCheck (MqBufferGetF (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetW
      inline MQ_WID GetW () const throw(MqCException) {
	MQ_WID out;
	ErrorCheck (MqBufferGetW (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetD
      inline MQ_DBL GetD () const throw(MqCException) {
	MQ_DBL out;
	ErrorCheck (MqBufferGetD (hdl, &out));
	return out;
      }

      /// \api #MqBufferGetC
      inline MQ_CST GetC () const throw(MqCException) {
	MQ_CST out;
	ErrorCheck (MqBufferGetC (hdl, &out));
	return out;
      }

      /// \return handle
      inline MQ_BUF GetU () const throw(MqCException) {
	return hdl;
      }

      /// \api #MqBufferGetB
      inline void GetB (MQ_BIN *outP, MQ_SIZE *sizeP) const throw(MqCException) {
	ErrorCheck (MqBufferGetB (hdl, outP, sizeP));
      }

      /// \api #MqBufferGetB
      inline vector<MQ_BINB>* GetB () const throw(MqCException) {
	MQ_BIN out;
	MQ_SIZE size;
	ErrorCheck (MqBufferGetB (hdl, &out, &size));
	return new vector<MQ_BINB> (out, out+size);
      }

      /// \api #MqBufferAppendC
      inline void AppendC (MQ_CST str) const {
	MqBufferAppendC (hdl, str);
      }

      /// \api #MqBufferReset
      inline void Reset () const throw(MqCException) {
	MqBufferReset (hdl);
      }
    /// \} Mq_Buffer_CC_API
  };

  /// \ingroup Mq_Context_CC_API
  /// \api #MqS
  class MqC {

    /// \defgroup Mq_Context_CC_API Mq_Context_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Context_C_API
    /// \details \copydetails Mq_Context_C_API
    /// \{

    public:
      /// \brief link between MqC and MqS
      struct MqS context;

    // Constructor

    public:
      /// \api #MqContextCreate
      MQ_EXTERN MqC (struct MqS * const tmpl);
      /// \api #MqContextCreate
      MQ_EXTERN MqC ();
      /// \api #MqContextDelete
      MQ_EXTERN virtual ~MqC ();

    public:
      /// \api #MqFactoryCtxDefaultSet
      inline void FactoryCtxDefaultSet (MQ_CST ident) throw(MqCException) { 
	ErrorCheck (MqFactoryCtxDefaultSet(&context, ident)); 
      }
      /// \api #MqFactoryCtxIdentSet
      inline void FactoryCtxIdentSet (MQ_CST ident) throw(MqCException) { 
	ErrorCheck (MqFactoryCtxIdentSet(&context, ident)); 
      }
      /// \api #MqFactoryCtxIdentGet
      inline MQ_CST FactoryCtxIdentGet () { 
	return MqFactoryCtxIdentGet(&context); 
      }
    public:
      typedef void (MqC::*CallbackF) ();

      MQ_EXTERN void objInit ();

    private:
      static inline MqC* GetThis(struct MqS const * const context) {
	return static_cast<MqC*>(context->self);
      }
  
      static void libInit (void) __attribute__ ((constructor));

      struct ProcCallS {
	enum ProcCallE	{
	  PC_CallbackF,
	  PC_IService,
	  PC_IEvent,
	  PC_IServerSetup,
	  PC_IServerCleanup,
	  PC_IBgError
	} type;
	union ProcCallU {
	  CallbackF	    Callback; 
	  IService *	    Service;
	  IEvent *	    Event;
	  IServerSetup *    ServerSetup;
	  IServerCleanup *  ServerCleanup;
	  IBgError *	    BgError;
	} call;
      };

      static enum MqErrorE MQ_DECL ProcCall (
	struct MqS * const context, 
	MQ_PTR const data
      );

      static void MQ_DECL ProcFree (
	struct MqS const * const context, 
	MQ_PTR *dataP
      );

      static void MQ_DECL ProcCopy (
	struct MqS * const context, 
	MQ_PTR * dataP
      );

      static void MQ_DECL ProcFreeISetup (
	struct MqS const * const context, 
	MQ_PTR *dataP
      );

    public:
      /// \brief get access to an internal buffer
      inline MQ_BUF GetTempBuffer() { return context.temp; }
      /// \api #MqExit
      inline void Exit () __attribute__((noreturn)) { MqExit(&context); } 
      /// \api #MqSysSleep
      inline void Sleep (unsigned int const sec) throw(MqCException) { ErrorCheck (MqSysSleep(&context, sec)); }
      /// \api #MqSysUSleep
      inline void USleep (unsigned int const usec) throw(MqCException) { ErrorCheck (MqSysUSleep(&context, usec)); }
      /// \api #MqLogC
      inline void LogC (MQ_CST const prefix, MQ_INT level, MQ_CST const str) { MqLogC(&context, prefix, level, str); }
      /// \api #MqLogV
      inline void LogV (MQ_CST const prefix, MQ_INT level, MQ_CST const fmt, ...) { 
	if (level > context.config.debug) {
	  return;
	} else {
	  va_list ap;
	  va_start (ap, fmt);
	  MqLogVL (&context, prefix, level, fmt, ap);
	  va_end (ap);
	}
      }
      /// \api #MqLogVL
      inline void LogVL (MQ_CST const prefix, MQ_INT level, MQ_CST const fmt, va_list ap) { MqLogVL(&context, prefix, level, fmt, ap); }

    /// \} Mq_Context_CC_API

    /// \defgroup Mq_Config_CC_API Mq_Config_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Config_C_API
    /// \details \copydetails Mq_Config_C_API
    /// \{
    public:

      // SET configuration data

      /// \api #MqConfigSetBuffersize
      inline void ConfigSetBuffersize (MQ_INT data)	    { MqConfigSetBuffersize (&context, data); }
      /// \api #MqConfigSetDebug
      inline void ConfigSetDebug      (MQ_INT data)	    { MqConfigSetDebug (&context, data); }
      /// \api #MqConfigSetTimeout
      inline void ConfigSetTimeout    (MQ_TIME_T data)	    { MqConfigSetTimeout (&context, data); }
      /// \api #MqConfigSetName
      inline void ConfigSetName	      (MQ_CST data)	    { MqConfigSetName (&context, data); }
      /// \api #MqConfigSetSrvName
      inline void ConfigSetSrvName    (MQ_CST data)	    { MqConfigSetSrvName (&context, data); }
      /// \api #MqConfigSetIsSilent
      inline void ConfigSetIsSilent   (MQ_BOL data)	    { MqConfigSetIsSilent (&context, data); }
      /// \api #MqConfigSetIsServer
      inline void ConfigSetIsServer   (MQ_BOL data)	    { MqConfigSetIsServer (&context, data); }
      /// \api #MqConfigSetIsString
      inline void ConfigSetIsString   (MQ_BOL data)	    { MqConfigSetIsString (&context, data); }
      /// \api #MqConfigSetIgnoreExit
      inline void ConfigSetIgnoreExit (MQ_BOL data)	    { MqConfigSetIgnoreExit (&context, data); }
      /// \api #MqConfigSetIoUdsFile
      inline void ConfigSetIoUdsFile  (MQ_CST data)	    { 
        ErrorCheck(MqConfigSetIoUdsFile (&context, data)); 
      }
      /// \api #MqConfigSetIoTcp
      inline void ConfigSetIoTcp      (MQ_CST host, MQ_CST port, MQ_CST myhost, MQ_CST myport)	{ 
	ErrorCheck(MqConfigSetIoTcp (&context, host, port, myhost, myport)); 
      }
      /// \api #MqConfigSetIoPipeSocket
      inline void ConfigSetIoPipeSocket     (MQ_SOCK data)	    { 
	ErrorCheck(MqConfigSetIoPipeSocket	 (&context, data)); 
      }
      /// \api #MqConfigSetStartAs
      inline void ConfigSetStartAs    (enum MqStartE data)  { MqConfigSetStartAs (&context, data); }
      /// \api #MqConfigSetDaemon
      inline void ConfigSetDaemon     (MQ_CST data) { 
	ErrorCheck (MqConfigSetDaemon  (&context, data)); 
      }

      // GET configuration data

      /// \api #MqConfigGetIsServer
      inline bool ConfigGetIsServer ()	    { return MqConfigGetIsServer(&context) == MQ_YES; }
      /// \api #MqConfigGetIsString
      inline bool ConfigGetIsString ()	    { return MqConfigGetIsString(&context) == MQ_YES; }
      /// \api #MqConfigGetIsSilent
      inline bool ConfigGetIsSilent ()	    { return MqConfigGetIsSilent(&context) == MQ_YES; }
      /// \api #MqConfigGetName
      inline MQ_CST ConfigGetName ()	    { return MqConfigGetName(&context); }
      /// \api #MqConfigGetSrvName
      inline MQ_CST ConfigGetSrvName ()	    { return MqConfigGetSrvName(&context); }
      /// \api #MqConfigGetBuffersize
      inline MQ_INT ConfigGetBuffersize ()  { return MqConfigGetBuffersize(&context); }
      /// \api #MqConfigGetDebug
      inline MQ_INT ConfigGetDebug ()	    { return MqConfigGetDebug(&context); }
      /// \api #MqConfigGetTimeout
      inline MQ_TIME_T ConfigGetTimeout ()  { return MqConfigGetTimeout(&context); }
      /// \api #MqConfigGetIoUdsFile
      inline MQ_CST ConfigGetIoUdsFile ()   { return MqConfigGetIoUdsFile (&context); }
      /// \api #MqConfigGetIoTcpHost
      inline MQ_CST ConfigGetIoTcpHost ()   { return MqConfigGetIoTcpHost (&context); }
      /// \api #MqConfigGetIoTcpPort
      inline MQ_CST ConfigGetIoTcpPort ()   { return MqConfigGetIoTcpPort (&context); }
      /// \api #MqConfigGetIoTcpMyHost
      inline MQ_CST ConfigGetIoTcpMyHost () { return MqConfigGetIoTcpMyHost (&context); }
      /// \api #MqConfigGetIoTcpMyPort
      inline MQ_CST ConfigGetIoTcpMyPort () { return MqConfigGetIoTcpMyPort (&context); }
      /// \api #MqConfigGetIoPipeSocket
      inline MQ_SOCK ConfigGetIoPipeSocket ()   { return MqConfigGetIoPipeSocket(&context); }
      /// \api #MqConfigGetStartAs
      inline enum MqStartE ConfigGetStartAs ()  { return MqConfigGetStartAs (&context); }
      /// \api #MqConfigGetStatusIs
      inline enum MqStatusIsE ConfigGetStatusIs ()  { return MqConfigGetStatusIs (&context); }
    /// \}

    /// \defgroup Mq_Link_CC_API Mq_Link_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Link_C_API
    /// \details \copydetails Mq_Link_C_API
    /// \{
    public:
      /// \api #MqLinkCreate
      inline void LinkCreateVC (int const argc, MQ_CST argv[]) {
	LinkCreate (MqBufferLCreateArgs (argc, argv));
      }
      /// \api #MqLinkCreate
      inline void LinkCreateVT ( const std::vector<MQ_CST>& args) {
	LinkCreateVC ((int const) args.size(), (MQ_CST*) &(*args.begin()));
      }
      /// \api #MqLinkCreate
      inline void LinkCreateVA (va_list ap) {
	LinkCreate (MqBufferLCreateArgsVA (&context, ap));
      }
      /// \api #MqLinkCreate
      MQ_EXTERN void _LinkCreateV (int dummy, ...);
      /// \api #MqLinkCreate
#     define LinkCreateV(...) _LinkCreateV(0, __VA_ARGS__)
      /// \api #MqLinkCreate
      MQ_EXTERN void LinkCreate (struct MqBufferLS * argv);

      /// \api #MqLinkCreateChild
      void LinkCreateChildVC (MqC& parent, int const argc, MQ_CST *argv ) {
	LinkCreateChild(parent, MqBufferLCreateArgs (argc, argv));
      }
      /// \api #MqLinkCreateChild
      void LinkCreateChildVA (MqC& parent, va_list ap) {
	LinkCreateChild(parent, MqBufferLCreateArgsVA(&context, ap));
      }
      /// \api #MqLinkCreateChild
      MQ_EXTERN void _LinkCreateChildV (MqC& parent, int dummy, ...); 
      /// \api #MqLinkCreateChild
#     define LinkCreateChildV(parent,...) _LinkCreateChildV(parent,0, __VA_ARGS__)
      /// \api #MqLinkCreateChild
      MQ_EXTERN void LinkCreateChild (MqC& parent, struct MqBufferLS * argv); 
      /// \api #MqLinkCreateChild
      inline void LinkCreateChildVT ( MqC& parent, std::vector<MQ_CST>& args ) {
	LinkCreateChildVC (parent, (int const ) args.size(), (MQ_CST*) &(*args.begin()));
      }

      /// \api #MqLinkDelete
      void LinkDelete () {
	MqLinkDelete (&context);
      }

      /// \api #MqLinkConnect
      void LinkConnect () {
	ErrorCheck (MqLinkConnect (&context));
      }

      /// \api #MqLinkGetParent 
      inline MqC* LinkGetParent ()	    { 
	struct MqS * const parent = MqLinkGetParentI(&context);
	return parent != NULL ? GetThis(parent) : NULL; 
      }

      /// \api #MqLinkIsParent
      inline bool LinkIsParent ()	    { 
        return MqLinkIsParentI(&context) == MQ_YES;
      }

      /// \api #MqLinkIsConnected
      inline bool LinkIsConnected ()   { 
        return MqLinkIsConnected(&context) == MQ_YES;
      }

      /// \api #MqLinkGetCtxId
      inline MQ_SIZE LinkGetCtxId ()  { 
        return MqLinkGetCtxIdI(&context); 
      }

      /// \api #MqLinkGetTargetIdent
      inline MQ_CST LinkGetTargetIdent () { 
	return MqLinkGetTargetIdent (&context);
      }

    /// \} Mq_Link_CC_API

    /// \defgroup Mq_Error_CC_API Mq_Error_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Error_C_API
    /// \details \copydetails Mq_Error_C_API
    /// \{

    public:

      inline void ErrorCheck(enum MqErrorE err) throw(MqCException) {
	if (MqErrorCheckI(err)) {
	  throw MqCException(&context);
	}
      }

      /// \api #MqErrorC
      inline void ErrorC (MQ_CST const prefix, MQ_INT const errnum, MQ_CST const message) {
	MqErrorC (&context, prefix, errnum, message);
      }

      /// \api #MqErrorV
      inline void ErrorV (MQ_CST const prefix, MQ_INT const errnum, MQ_CST const fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	MqErrorSGenVL (&context, prefix, MQ_ERROR, errnum, fmt, ap);
	va_end (ap);
      }

      /// \brief throw an #ccmsgque::MqCException using the data from #MqErrorS
      inline void ErrorRaise () throw(MqCException) {
	ErrorCheck (MqErrorGetCodeI (&context));
      }

      /// \api #MqErrorReset
      inline void ErrorReset () {
	MqErrorReset (&context);
      }

      /// \api #MqErrorPrint
      inline void ErrorPrint () {
	MqErrorPrint (&context);
      }

      /// \api #MqErrorSetCONTINUE
      inline void ErrorSetCONTINUE () {
	MqErrorSetCONTINUE (&context);
      }

      /// \api #MqErrorSetEXIT
      inline void ErrorSetEXIT () {
	ErrorCheck (MqErrorSetEXIT (&context));
      }

      /// \api #MqErrorGetNum
      inline int ErrorGetNum () {
	return MqErrorGetNumI (&context);
      }

      /// \api #MqErrorGetText
      inline MQ_CST ErrorGetText () {
	return MqErrorGetText (&context);
      }

      /// \api #MqErrorSet
      MQ_EXTERN enum MqErrorE ErrorSet (const exception& e);

      /// \api #MqErrorIsEXIT
      inline bool ErrorIsEXIT () {
	return MqErrorIsEXIT (&context) == MQ_YES;
      }

    /// \}

    /// \defgroup Mq_Read_CC_API Mq_Read_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Read_C_API
    /// \details \copydetails Mq_Read_C_API
    /// \{
    public:
      /// \api #MqReadY
      inline MQ_BYT ReadY() throw(MqCException) { MQ_BYT val; ErrorCheck (MqReadY(&context, &val)); return val; }
      /// \api #MqReadO
      inline MQ_BOL ReadO() throw(MqCException) { MQ_BOL val; ErrorCheck (MqReadO(&context, &val)); return val; }
      /// \api #MqReadS
      inline MQ_SRT ReadS() throw(MqCException) { MQ_SRT val; ErrorCheck (MqReadS(&context, &val)); return val; }
      /// \api #MqReadI
      inline MQ_INT ReadI() throw(MqCException) { MQ_INT val; ErrorCheck (MqReadI(&context, &val)); return val; }
      /// \api #MqReadF
      inline MQ_FLT ReadF() throw(MqCException) { MQ_FLT val; ErrorCheck (MqReadF(&context, &val)); return val; }
      /// \api #MqReadW
      inline MQ_WID ReadW() throw(MqCException) { MQ_WID val; ErrorCheck (MqReadW(&context, &val)); return val; }
      /// \api #MqReadD
      inline MQ_DBL ReadD() throw(MqCException) { MQ_DBL val; ErrorCheck (MqReadD(&context, &val)); return val; }
      /// \api #MqReadC
      inline MQ_CST ReadC() throw(MqCException) { 
	MQ_CST val; ErrorCheck (MqReadC(&context, &val)); return val; 
      }
      /// \api #MqReadB
      inline void ReadB(MQ_BIN * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadB(&context, valP, lenP)); 
      }
      /// \api #MqReadB
      inline vector<MQ_BINB>* ReadB() throw(MqCException) { 
	MQ_BIN val; 
	MQ_SIZE len;
	ErrorCheck (MqReadB(&context, &val, &len)); 
	return new vector<MQ_BINB> (val, val+len);
      }
      /// \api #MqReadN
      inline void ReadN(MQ_CBI * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadN(&context, valP, lenP)); 
      }
      /// \api #MqReadBDY
      inline void ReadBDY(MQ_BIN * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadBDY(&context, valP, lenP)); 
      }
      /// \api #MqReadU
      inline MQ_BUF ReadU() throw(MqCException) 
	{ MQ_BUF val; ErrorCheck (MqReadU(&context, &val)); return val; }
      /// \api #MqReadProxy
      inline void ReadProxy(MqC& ctx) throw(MqCException) 
	{ ErrorCheck (MqReadProxy(&context, &ctx.context)); }
      /// \api #MqReadProxy
      inline void ReadProxy(MqC * const ctx) throw(MqCException) 
	{ ErrorCheck (MqReadProxy(&context, &ctx->context)); }

      /// \api #MqReadL_START
      inline void ReadL_START(MQ_BUF buf = NULL) throw(MqCException) 
	{ ErrorCheck (MqReadL_START(&context, buf)); }
      /// \api #MqReadL_START
      inline void ReadL_START(const MqBufferC& buf) throw(MqCException) 
	{ ErrorCheck (MqReadL_START(&context, buf.GetU())); }
      /// \api #MqReadL_START
      inline void ReadL_START(MqBufferC * const buf) throw(MqCException)
	{ ErrorCheck (MqReadL_START(&context, buf->GetU())); }
      /// \api #MqReadL_END
      inline void ReadL_END() throw(MqCException) 
	{ ErrorCheck (MqReadL_END(&context)); }

      /// \api #MqReadT_START
      inline void ReadT_START(MQ_BUF buf = NULL) throw(MqCException) 
	{ ErrorCheck (MqReadT_START(&context, buf)); }
      /// \api #MqReadT_START
      inline void ReadT_START(const MqBufferC& buf) throw(MqCException) 
	{ ErrorCheck (MqReadT_START(&context, buf.GetU())); }
      /// \api #MqReadT_START
      inline void ReadT_START(MqBufferC * const buf) throw(MqCException)
	{ ErrorCheck (MqReadT_START(&context, buf->GetU())); }
      /// \api #MqReadT_END
      inline void ReadT_END() throw(MqCException) 
	{ ErrorCheck (MqReadT_END(&context)); }

      /// \api #MqReadItemExists
      inline MQ_BOL  ReadItemExists()		
	{return MqReadItemExists(&context);}
      /// \api #MqReadGetNumItems
      inline MQ_SIZE ReadGetNumItems()		
	{return MqReadGetNumItems(&context);}
      /// \api #MqReadUndo
      inline void ReadUndo() throw(MqCException)	
	{return ErrorCheck (MqReadUndo(&context)); }

    /// \} Mq_Read_CC_API

    /// \defgroup Mq_Send_CC_API Mq_Send_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Send_C_API
    /// \details \copydetails Mq_Send_C_API
    /// \{

    public:
      /// \api #MqSendSTART
      inline void SendSTART () throw(MqCException)	  { ErrorCheck(MqSendSTART(&context)); }
      /// \api #MqSendRETURN
      inline void SendRETURN () throw(MqCException)	  { ErrorCheck(MqSendRETURN(&context)); }
      /// \api #MqSendERROR
      inline void SendERROR () throw(MqCException)	  { ErrorCheck(MqSendERROR(&context)); }
      /// \api #MqSendEND
      inline void SendEND (MQ_CST const token) throw(MqCException) { 
	ErrorCheck(MqSendEND(&context, token)); 
      }
      /// \api #MqSendEND_AND_WAIT
      inline void SendEND_AND_WAIT (MQ_CST const token, MQ_TIME_T timeout = MQ_TIMEOUT_USER) throw(MqCException) { 
	ErrorCheck(MqSendEND_AND_WAIT(&context, token, timeout)); 
      }
      /// \api #MqSendEND_AND_CALLBACK
      MQ_EXTERN void SendEND_AND_CALLBACK (MQ_CST const token, CallbackF const callback) throw(MqCException);
      /// \api #MqSendEND_AND_CALLBACK
      MQ_EXTERN void SendEND_AND_CALLBACK (MQ_CST const token, IService * const callback) throw(MqCException);

      /// \api #MqSendY
      inline void SendY (MQ_BYT val) throw(MqCException) { ErrorCheck (MqSendY (&context, val)); }
      /// \api #MqSendO
      inline void SendO (MQ_BOL val) throw(MqCException) { ErrorCheck (MqSendO (&context, val)); }
      /// \api #MqSendS
      inline void SendS (MQ_SRT val) throw(MqCException) { ErrorCheck (MqSendS (&context, val)); }
      /// \api #MqSendI
      inline void SendI (MQ_INT val) throw(MqCException) { ErrorCheck (MqSendI (&context, val)); }
      /// \api #MqSendF
      inline void SendF (MQ_FLT val) throw(MqCException) { ErrorCheck (MqSendF (&context, val)); }
      /// \api #MqSendW
      inline void SendW (MQ_WID val) throw(MqCException) { ErrorCheck (MqSendW (&context, val)); }
      /// \api #MqSendD
      inline void SendD (MQ_DBL val) throw(MqCException) { ErrorCheck (MqSendD (&context, val)); }
      /// \api #MqSendC
      inline void SendC (MQ_CST val) throw(MqCException) { ErrorCheck (MqSendC (&context, val)); }
      /// \api #MqSendC
      inline void SendV (MQ_CST fmt, ...) throw(MqCException) { 
	enum MqErrorE ret;
	va_list ap;
	va_start(ap, fmt);
	ret = MqSendVL (&context, fmt, ap); 
	va_end(ap);
	ErrorCheck (ret);
      }
      /// \api #MqSendU
      inline void SendU (MQ_BUF val) throw(MqCException) { ErrorCheck (MqSendU (&context, val)); }
      /// \api #MqSendN
      inline void SendN (MQ_CBI val, MQ_SIZE len) throw(MqCException) { ErrorCheck (MqSendN (&context, val, len)); }
      /// \api #MqSendBDY
      inline void SendBDY (MQ_CBI val, MQ_SIZE len) throw(MqCException) { 
	ErrorCheck (MqSendBDY (&context, val, len));
      }
      /// \api #MqSendB
      inline void SendB (MQ_CBI const val, MQ_SIZE const len) throw(MqCException) { ErrorCheck (MqSendB (&context, val, len)); }
      /// \api #MqSendB
      inline void SendB (vector<MQ_BINB>* val) throw(MqCException) { 
	ErrorCheck (MqSendB (&context, &(*val->begin()) , (int) val->size())); 
      }
      /// \api #MqSendL_START
      inline void SendL_START() throw(MqCException)      { ErrorCheck (MqSendL_START(&context)); }
      /// \api #MqSendL_END
      inline void SendL_END() throw(MqCException)	 { ErrorCheck (MqSendL_END(&context)); }
      /// \api #MqSendT_START
      inline void SendT_START(MQ_CST token) throw(MqCException)	{ ErrorCheck (MqSendT_START(&context, token)); }
      /// \api #MqSendT_END
      inline void SendT_END() throw(MqCException)	 { ErrorCheck (MqSendT_END(&context)); }

    /// \} Mq_Send_CC_API

    /// \defgroup Mq_Service_CC_API Mq_Service_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Service_C_API
    /// \details \copydetails Mq_Service_C_API
    /// \{
    public:
      /// \api #MqServiceIsTransaction
      inline bool ServiceIsTransaction () { return MqServiceIsTransaction(&context) == MQ_YES;}

      /// \api #MqServiceGetFilter
      inline MqC* ServiceGetFilter (MQ_SIZE id=0) throw(MqCException)  { 
	struct MqS* val; 
	ErrorCheck (MqServiceGetFilter(&context, id, &val));
	return GetThis(val);
      }

      /// \api #MqServiceGetToken
      inline MQ_CST ServiceGetToken ()	    { return MqServiceGetToken(&context); }

      /// \api #MqServiceCreate
      MQ_EXTERN void ServiceCreate(MQ_CST const token, CallbackF const callback) throw(MqCException);

      /// \api #MqServiceCreate
      MQ_EXTERN void ServiceCreate(MQ_CST const token, IService * const service) throw(MqCException);

      /// \api #MqServiceDelete
      MQ_EXTERN void ServiceDelete(MQ_CST const token) throw(MqCException);

      /// \api #MqServiceProxy
      inline void ServiceProxy (MQ_CST const token, MQ_SIZE id=0) throw(MqCException) { 
	ErrorCheck (MqServiceProxy (&context, token, id));
      }

      /// \api #MqProcessEvent
      inline void ProcessEvent (
	MQ_TIME_T		    timeout,
	enum MqWaitOnEventE const   wait
      ) throw(MqCException)
      {
	ErrorCheck(MqProcessEvent(&context, timeout, wait));
      }
      /// \api #MqProcessEvent
      inline void ProcessEvent (
	enum MqWaitOnEventE const   wait      = MQ_WAIT_ONCE
      ) throw(MqCException)
      {
	ErrorCheck(MqProcessEvent(&context, MQ_TIMEOUT_DEFAULT, wait));
      }

    /// \} Mq_Service_CC_API

    /// \defgroup Mq_Slave_CC_API Mq_Slave_CC_API
    /// \ingroup Mq_CC_API
    /// \brief \copybrief Mq_Slave_C_API
    /// \details \copydetails Mq_Slave_C_API
    /// \{

    public:
      /// \api #MqSlaveGet
      inline MqC *SlaveGet (const int id) { 
	struct MqS * const slave = MqSlaveGet(&context,id);
	return slave ? GetThis(slave) : NULL;
      }

      /// \api #MqSlaveGetMaster
      inline MqC* SlaveGetMaster ()	    { 
	struct MqS * const master = MqSlaveGetMasterI(&context);
	return master ? GetThis(master) : NULL; 
      }

      /// \api #MqSlaveIs
      inline bool SlaveIs ()	    { 
        return MqSlaveIs(&context) == MQ_YES;
      }

      /// \api #MqSlaveCreate
      inline void SlaveCreate (const int id, MqC * const slave) throw(MqCException) {
	enum MqErrorE ret = MqSlaveCreate(&context,id,&slave->context);
	ErrorCheck (ret);
      }

      /// \api #MqSlaveWorker
      void SlaveWorker (const int id, struct MqBufferLS * args = NULL) {
	ErrorCheck (MqSlaveWorker (&context, id, &args));
      }

      /// \api #MqSlaveWorker
      inline void SlaveWorkerVC (const int id, int const argc, MQ_CST *argv) {
	SlaveWorker (id, MqBufferLCreateArgsVC(&context, argc, argv));
      }

      /// \api #MqSlaveWorker
      inline void SlaveWorkerVA (const int id, va_list ap) {
	SlaveWorker (id, MqBufferLCreateArgsVA(&context, ap));
      }

      /// \api #MqSlaveWorker
      MQ_EXTERN void SlaveWorkerV (const int id, ...);

      /// \api #MqSlaveWorker
      inline void SlaveWorkerVT (const int id, vector<MQ_CST>& args) {
	SlaveWorkerVC (id, (const int) args.size(), (MQ_CST*)&(*args.begin()));
      }

      /// \api #MqSlaveDelete
      inline void SlaveDelete (const int id) throw(MqCException) { 
	ErrorCheck (MqSlaveDelete(&context,id)); 
      }
    /// \} Mq_Slave_CC_API
  };

  /// \defgroup Mq_Factory_CC_API Mq_Factory_CC_API
  /// \ingroup Mq_CC_API
  /// \brief \copybrief MqFactory
  /// \details \copydetails MqFactory
  /// \{

  class MqFactoryCException : public exception
  {
    private:
      enum MqFactoryReturnE err;
    public:
      MqFactoryCException(enum MqFactoryReturnE ret) : err (ret) { }
      virtual MQ_CST what() const throw() { return MqFactoryErrorMsg(err); }
  };

  template <typename T>
  class MqFactoryC {

    private:
      static inline void ErrorCheck(enum MqFactoryReturnE ret) throw(MqCException) {
	if (MqFactoryErrorCheckI(ret)) throw MqFactoryCException(ret);
      }
      static inline T* GetThis(struct MqS const * const context) {
	return static_cast<T*>(context->self);
      }
      static enum MqErrorE FactoryCreate ( 
	struct MqS * const tmpl, 
	enum MqFactoryE create, 
	struct MqFactoryS * item, 
	struct MqS  ** contextP
      ) {
	try { 
	  struct MqS * mqctx = &static_cast<T*const>(new T(tmpl))->context;
	  if (MqErrorCheckI(MqErrorGetCode(mqctx))) {
	    *contextP = NULL;
	    if (create != MQ_FACTORY_NEW_INIT) {
	      MqErrorCopy (tmpl, mqctx);
	      MqContextDelete (&mqctx);
	      return MqErrorStack(tmpl);
	    } else {
	      return MQ_ERROR;
	    }
	  }
	  GetThis(mqctx)->objInit();
	  if (create != MQ_FACTORY_NEW_INIT) {
	    MqSetupDup(mqctx, tmpl);
	  }
	  *contextP = mqctx;
	} catch (exception& ex) {
	  *contextP = NULL;
	  if (create != MQ_FACTORY_NEW_INIT) {
	    return GetThis(tmpl)->ErrorSet(ex);
	  } else {
	    return MQ_ERROR;
	  }
	} catch (...) {
	  *contextP = NULL;
	  if (create != MQ_FACTORY_NEW_INIT) {
	    return MqErrorC(tmpl, __func__, -1, "Factory return no MqS type");
	  } else {
	    return MQ_ERROR;
	  }
	}
	return MQ_OK;
      }
      static void FactoryDelete (
	struct MqS * context,
	MQ_BOL doFactoryCleanup,
	struct MqFactoryS * const item
      ) {
	if (doFactoryCleanup == MQ_YES) {
	  delete GetThis(context);
	} else {
	  MqContextFree(context);
	}
      };

    public:
      /// \api #MqFactoryAdd
      static inline void Add(MQ_CST ident) throw (MqFactoryCException) {
	ErrorCheck (MqFactoryAdd (ident, FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL));
      }
      /// \api #MqFactoryAdd
      static inline void Add() throw (MqFactoryCException) {
	ErrorCheck (MqFactoryAdd (typeid(T).name(), FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL));
      }
      /// \api #MqFactoryDefault
      static inline void Default(MQ_CST ident) throw (MqFactoryCException) {
	ErrorCheck (MqFactoryDefault (ident, FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL));
      }
      /// \api #MqFactoryDefault
      static inline void Default() throw (MqFactoryCException) {
	ErrorCheck (MqFactoryDefault (typeid(T).name(), FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL));
      }
      /// \api #MqFactoryDefaultIdent
      static inline MQ_CST DefaultIdent() {
	return MqFactoryDefaultIdent();
      }
      /// \api #MqFactoryCall
      static inline T* Call(MQ_CST ident) throw (MqFactoryCException) {
	struct MqS *mqctx;
	ErrorCheck (MqFactoryCall (ident, NULL, &mqctx));
	return static_cast<T*>(mqctx->self);
      }
      /// \api #MqFactoryCall
      static inline T* Call() throw (MqFactoryCException) {
	struct MqS *mqctx;
	ErrorCheck (MqFactoryCall (typeid(T).name(), NULL, &mqctx));
	return static_cast<T*>(mqctx->self);
      }
      /// \api #MqFactoryNew
      static inline T* New(MQ_CST ident) throw (MqFactoryCException) {
	struct MqS *mqctx;
	ErrorCheck (MqFactoryNew (ident, FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL, NULL, &mqctx));
	return static_cast<T*>(mqctx->self);
      }
      /// \api #MqFactoryNew
      static inline T* New() throw (MqFactoryCException) {
	struct MqS *mqctx;
	ErrorCheck (MqFactoryNew (typeid(T).name(), FactoryCreate, NULL, NULL, FactoryDelete, NULL, NULL, NULL, &mqctx));
	return static_cast<T*>(mqctx->self);
      }
  };

  /// \} Mq_Factory_CC_API

};  // END - namespace "ccmsgque"

/// \} Mq_CC_API

#endif /* MQ_CCMSGQUE_H */

