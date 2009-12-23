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

#if !defined(MQ_COMPILE_AS_CC)
# define MQ_COMPILE_AS_CC
#endif
#include "msgque.h"
#include "debug.h"

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

  /// \sameas{MqConfigSetFactory}
  ///
  /// The \e Factory is used to create a:
  ///  - \e server-child object for \e context management
  ///  - \e client-child object for \e slave management
  ///  .
  /// \code
  /// class MyServer : public IFactory {
  ///   ....
  ///   MqC* Factory() const {return new MyServer();}
  ///   ....
  /// }
  /// \endcode
  class IFactory {
    public:
      /// \brief create a object of the top-most class
      virtual MqC* Factory () const = 0;
  };

  /// \sameas{MqConfigSetServerSetup}
  class IServerSetup {
    public:
      virtual void ServerSetup () = 0;
  };

  /// \sameas{MqConfigSetServerCleanup}
  class IServerCleanup {
    public:
      virtual void ServerCleanup () = 0;
  };

  /// \sameas{MqConfigSetBgError}
  class IBgError {
    public:
      virtual void BgError () = 0;
  };

  /// \brief a C++ error class wrapper for the \e MqErrorS struct
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

  /// \api #MqBufferS
  class MqBufferC {
    private:
      struct MqBufferS * hdl;

      inline void ErrorCheck(enum MqErrorE err) const throw(MqCException) {
	if (MqErrorCheckI(err)) throw MqCException(hdl->context);
      }

    public:

      MqBufferC (struct MqBufferS *bufP) {
	hdl = bufP;
      }

      inline enum MqTypeE GetType () const {
	return hdl->type;
      }

      inline MQ_BYT GetY () const throw(MqCException) {
	MQ_BYT out;
	ErrorCheck (MqBufferGetY (hdl, &out));
	return out;
      }

      inline MQ_BOL GetO () const throw(MqCException) {
	MQ_BOL out;
	ErrorCheck (MqBufferGetO (hdl, &out));
	return out;
      }

      inline MQ_SRT GetS () const throw(MqCException) {
	MQ_SRT out;
	ErrorCheck (MqBufferGetS (hdl, &out));
	return out;
      }

      inline MQ_INT GetI () const throw(MqCException) {
	MQ_INT out;
	ErrorCheck (MqBufferGetI (hdl, &out));
	return out;
      }

      inline MQ_FLT GetF () const throw(MqCException) {
	MQ_FLT out;
	ErrorCheck (MqBufferGetF (hdl, &out));
	return out;
      }

      inline MQ_WID GetW () const throw(MqCException) {
	MQ_WID out;
	ErrorCheck (MqBufferGetW (hdl, &out));
	return out;
      }

      inline MQ_DBL GetD () const throw(MqCException) {
	MQ_DBL out;
	ErrorCheck (MqBufferGetD (hdl, &out));
	return out;
      }

      inline MQ_CST GetC () const throw(MqCException) {
	MQ_CST out;
	ErrorCheck (MqBufferGetC (hdl, &out));
	return out;
      }

      inline MQ_BUF GetU () const throw(MqCException) {
	return hdl;
      }

      inline void GetB (MQ_BIN *outP, MQ_SIZE *sizeP) const throw(MqCException) {
	ErrorCheck (MqBufferGetB (hdl, outP, sizeP));
      }

      inline vector<MQ_BINB>* GetB () const throw(MqCException) {
	MQ_BIN out;
	MQ_SIZE size;
	ErrorCheck (MqBufferGetB (hdl, &out, &size));
	return new vector<MQ_BINB> (out, out+size);
      }

      inline void AppendC (MQ_CST str) const {
	MqBufferAppendC (hdl, str);
      }

      inline void Reset () const throw(MqCException) {
	MqBufferReset (hdl);
      }
  };

  /// \brief main ccmsgque class
  class MqC {

    private:
      /// \brief link between MqC and MqS
      struct MqS context;

    // Constructor

    public:
      MQ_EXTERN MqC ();
      MQ_EXTERN virtual ~MqC ();

    public:
      /// \brief class-method-callback
      typedef void (MqC::*CallbackF) ();
      typedef MqC* (MqC::*FactoryF) ();

    private:
      static inline MqC* GetThis(struct MqS const * const context) {
	return static_cast<MqC*>(context->self);
      }

    private:
      MQ_EXTERN void Init ();
      
      static enum MqErrorE FactoryCreate (
	struct MqS * const tmpl,
	enum MqFactoryE create,
	MQ_PTR data,
	struct MqS  ** contextP
      );

      static void FactoryDelete (
	struct MqS * ctx,
      MQ_BOL doFactoryCleanup,
	MQ_PTR data
      );

      struct ProcCallS {
	enum ProcCallE	{
	  PC_CallbackF,
	  PC_IService,
	  PC_IServerSetup,
	  PC_IServerCleanup,
	  PC_IBgError
	} type;
	union ProcCallU {
	  CallbackF	    Callback; 
	  IService *	    Service;
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

    /// \defgroup config_API Access to Configuration data
    /// \{
    public:

      // SET configuration data
      inline void ConfigSetBuffersize (MQ_INT data)	    { MqConfigSetBuffersize (&context, data); }
      inline void ConfigSetDebug      (MQ_INT data)	    { MqConfigSetDebug (&context, data); }
      inline void ConfigSetTimeout    (MQ_TIME_T data)	    { MqConfigSetTimeout (&context, data); }
      inline void ConfigSetName	      (MQ_CST data)	    { MqConfigSetName (&context, data); }
      inline void ConfigSetSrvName    (MQ_CST data)	    { MqConfigSetSrvName (&context, data); }
      inline void ConfigSetIdent      (MQ_CST data)	    { MqConfigSetIdent (&context, data); }
      inline MQ_BOL ConfigCheckIdent  (MQ_CST data)	    { return MqConfigCheckIdent (&context, data); }
      inline void ConfigSetIsSilent   (MQ_BOL data)	    { MqConfigSetIsSilent (&context, data); }
      inline void ConfigSetIsServer   (MQ_BOL data)	    { MqConfigSetIsServer (&context, data); }
      inline void ConfigSetIsString   (MQ_BOL data)	    { MqConfigSetIsString (&context, data); }
      inline void ConfigSetIoUds      (MQ_CST data)	    { 
        ErrorCheck(MqConfigSetIoUds (&context, data)); 
      }
      inline void ConfigSetIoTcp      (MQ_CST host, MQ_CST port, MQ_CST myhost, MQ_CST myport)	{ 
	ErrorCheck(MqConfigSetIoTcp (&context, host, port, myhost, myport)); 
      }
      inline void ConfigSetIoPipe     (MQ_SOCK data)	    { 
	ErrorCheck(MqConfigSetIoPipe	 (&context, data)); 
      }
      inline void ConfigSetStartAs    (enum MqStartE data)  { MqConfigSetStartAs (&context, data); }
      inline void ConfigSetDaemon     (MQ_CST data) { 
	ErrorCheck (MqConfigSetDaemon  (&context, data)); 
      }

      // GET configuration data

      inline bool ConfigGetIsServer ()	    { return MqConfigGetIsServer(&context) != 0; }
      inline bool ConfigGetIsParent ()	    { return MqConfigGetIsParent(&context) != 0; }
      inline bool ConfigGetIsSlave ()	    { return MqConfigGetIsSlave(&context) != 0; }
      inline bool ConfigGetIsString ()	    { return context.config.isString == MQ_YES; }
      inline bool ConfigGetIsSilent ()	    { return context.config.isSilent == MQ_YES; }
      inline bool ConfigGetIsConnected ()   { return (context.link.onCreate == MQ_YES); }
      inline bool ConfigGetIsTransaction () { return MqConfigGetIsTransaction(&context) ? true : false;}
      inline MQ_CST ConfigGetName ()	    { return context.config.name; }
      inline MQ_CST ConfigGetSrvName ()	    { return context.config.srvname; }
      inline MQ_CST ConfigGetIdent ()	    { return context.setup.ident; }
      inline MQ_CST ConfigGetToken ()	    { return MqConfigGetToken(&context); }
      inline MQ_INT ConfigGetBuffersize ()  { return context.config.io.buffersize; }
      inline MQ_INT ConfigGetDebug ()	    { return context.config.debug; }
      inline MQ_TIME_T ConfigGetTimeout ()  { return context.config.io.timeout; }
      inline MqC* ConfigGetParent ()	    { return context.config.parent ? GetThis(context.config.parent) : NULL; }
      inline MqC* ConfigGetMaster ()	    { return context.config.master ? GetThis(context.config.master) : NULL; }
      inline MqC* ConfigGetFilter (MQ_SIZE id=0) throw(MqCException)  { 
	struct MqS* val; 
	ErrorCheck (MqConfigGetFilter(&context, id, &val));
	return GetThis(val);
      }
      inline int ConfigGetCtxId ()	    { return MqConfigGetCtxId(&context); };
      inline MQ_CST ConfigGetIoUdsFile ()   { return MqConfigGetIoUdsFile (&context); }
      inline MQ_CST ConfigGetIoTcpHost ()   { return MqConfigGetIoTcpHost (&context); }
      inline MQ_CST ConfigGetIoTcpPort ()   { return MqConfigGetIoTcpPort (&context); }
      inline MQ_CST ConfigGetIoTcpMyHost () { return MqConfigGetIoTcpMyHost (&context); }
      inline MQ_CST ConfigGetIoTcpMyPort () { return MqConfigGetIoTcpMyPort (&context); }
      inline MQ_SOCK ConfigGetIoPipeSocket ()   { return MqConfigGetIoPipeSocket(&context); }
      inline enum MqStartE ConfigGetStartAs ()  { return MqConfigGetStartAs (&context); }
    /// \}

    /// \defgroup Error_API Error? : Create a Error
    /// \{

    public:

      /// \brief check method return value on error
      inline void ErrorCheck(enum MqErrorE err) throw(MqCException) {
	if (MqErrorCheckI(err)) {
	  throw MqCException(&context);
	}
      }

      inline void ErrorC (MQ_CST const prefix, MQ_INT const errnum, MQ_CST const message) {
	MqErrorC (&context, prefix, errnum, message);
      }

      inline void ErrorV (MQ_CST const prefix, MQ_INT const errnum, MQ_CST const fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	MqErrorSGenVL (&context, prefix, MQ_ERROR, errnum, fmt, ap);
	va_end (ap);
      }

      inline void ErrorRaise () throw(MqCException) {
	ErrorCheck (MqErrorGetCode (&context));
      }

      inline void ErrorReset () {
	MqErrorReset (&context);
      }

      inline void ErrorPrint () {
	MqErrorPrint (&context);
      }

      inline int ErrorGetNum () {
	return MqErrorGetNum (&context);
      }

      inline MQ_CST ErrorGetText () {
	return MqErrorGetText (&context);
      }

      /// \brief convert a C++ "exception" into a "MqCException"
      MQ_EXTERN enum MqErrorE ErrorSet (const exception& e);

    /// \}

    /// \defgroup setup_API Link? : Create and Delete a Link
    /// \{
    public:

    // PARENT

      /// create a \e parent object
      void LinkCreateVC (int const argc, MQ_CST argv[]) {
	LinkCreate (MqBufferLCreateArgs (argc, argv));
      }
      /// create a \e parent object from \e vector
      inline void LinkCreateVT ( const std::vector<MQ_CST>& args) {
	LinkCreateVC ((int const) args.size(), (MQ_CST*) &(*args.begin()));
      }
      /// create a \e parent object from \e var_arg
      void LinkCreateVA (va_list ap) {
	LinkCreate (MqBufferLCreateArgsVA (&context, ap));
      }
      /// create a \e parent object from \e var_arg
      MQ_EXTERN void _LinkCreateV (int dummy, ...);
#     define LinkCreateV(...) _LinkCreateV(0, __VA_ARGS__)
      /// \sameas{MqLinkCreate} -> create a \e parent object from \e #MqBufferLS
      MQ_EXTERN void LinkCreate (struct MqBufferLS * argv);

    // CHILD

      /// create a \e child object
      void LinkCreateChildVC (MqC& parent, int const argc, MQ_CST *argv ) {
	LinkCreateChild(parent, MqBufferLCreateArgs (argc, argv));
      }
      /// create a \e child object from \e var_arg
      void LinkCreateChildVA (MqC& parent, va_list ap) {
	LinkCreateChild(parent, MqBufferLCreateArgsVA(&context, ap));
      }
      /// create a \e child object from \e var_arg
      MQ_EXTERN void _LinkCreateChildV (MqC& parent, int dummy, ...); 
#     define LinkCreateChildV(parent,...) _LinkCreateChildV(parent,0, __VA_ARGS__)
      /// create a \e child object from \e #MqBufferLS
      MQ_EXTERN void LinkCreateChild (MqC& parent, struct MqBufferLS * argv); 
      /// create a \e child object from \e vector
      inline void LinkCreateChildVT ( MqC& parent, std::vector<MQ_CST>& args ) {
	LinkCreateChildVC (parent, (int const ) args.size(), (MQ_CST*) &(*args.begin()));
      }

      void LinkDelete () {
	MqLinkDelete (&context);
      }

    /// \}

    // Misc


    /// \defgroup misc_API Misc
    /// \{
    public:

      /// \sameas{MqProcessEvent}
      MQ_EXTERN void ProcessEvent (
	MQ_TIME_T		    timeout   = MQ_TIMEOUT_USER,
	enum MqWaitOnEventE const   wait      = MQ_WAIT_FOREVER
      ) throw(MqCException);

      inline MQ_BUF GetTempBuffer() { return context.temp; }
      inline void Exit () __attribute__((noreturn)) { MqExit (&context); } 
      inline void Sleep (unsigned int const sec) throw(MqCException) { ErrorCheck (MqSysSleep(&context, sec)); }
      inline void USleep (unsigned int const usec) throw(MqCException) { ErrorCheck (MqSysUSleep(&context, usec)); }
    /// \}

    /// \defgroup read_API Read? : Read data from a ccmsgque Package
    /// \{
    public:
      inline MQ_BYT ReadY() throw(MqCException) { MQ_BYT val; ErrorCheck (MqReadY(&context, &val)); return val; }
      inline MQ_BOL ReadO() throw(MqCException) { MQ_BOL val; ErrorCheck (MqReadO(&context, &val)); return val; }
      inline MQ_SRT ReadS() throw(MqCException) { MQ_SRT val; ErrorCheck (MqReadS(&context, &val)); return val; }
      inline MQ_INT ReadI() throw(MqCException) { MQ_INT val; ErrorCheck (MqReadI(&context, &val)); return val; }
      inline MQ_FLT ReadF() throw(MqCException) { MQ_FLT val; ErrorCheck (MqReadF(&context, &val)); return val; }
      inline MQ_WID ReadW() throw(MqCException) { MQ_WID val; ErrorCheck (MqReadW(&context, &val)); return val; }
      inline MQ_DBL ReadD() throw(MqCException) { MQ_DBL val; ErrorCheck (MqReadD(&context, &val)); return val; }
      inline MQ_CST ReadC() throw(MqCException) { 
	MQ_CST val; ErrorCheck (MqReadC(&context, &val)); return val; 
      }
      inline void ReadB(MQ_BIN * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadB(&context, valP, lenP)); 
      }
      inline vector<MQ_BINB>* ReadB() throw(MqCException) { 
	MQ_BIN val; 
	MQ_SIZE len;
	ErrorCheck (MqReadB(&context, &val, &len)); 
	return new vector<MQ_BINB> (val, val+len);
      }
      inline void ReadN(MQ_BIN * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadN(&context, valP, lenP)); 
      }
      inline void ReadBDY(MQ_BIN * const valP, MQ_SIZE * const lenP) throw(MqCException) { 
	ErrorCheck (MqReadBDY(&context, valP, lenP)); 
      }
      inline MQ_BUF ReadU() throw(MqCException) 
	{ MQ_BUF val; ErrorCheck (MqReadU(&context, &val)); return val; }
      inline void ReadProxy(MqC& ctx) throw(MqCException) 
	{ ErrorCheck (MqReadProxy(&context, &ctx.context)); }
      inline void ReadProxy(MqC * const ctx) throw(MqCException) 
	{ ErrorCheck (MqReadProxy(&context, &ctx->context)); }

      inline void ReadL_START(MQ_BUF buf = NULL) throw(MqCException) 
	{ ErrorCheck (MqReadL_START(&context, buf)); }
      inline void ReadL_START(const MqBufferC& buf) throw(MqCException) 
	{ ErrorCheck (MqReadL_START(&context, buf.GetU())); }
      inline void ReadL_START(MqBufferC * const buf) throw(MqCException)
	{ ErrorCheck (MqReadL_START(&context, buf->GetU())); }
      inline void ReadL_END() throw(MqCException) 
	{ ErrorCheck (MqReadL_END(&context)); }

      inline MQ_BOL  ReadItemExists()		
	{return MqReadItemExists(&context);}
      inline MQ_SIZE ReadGetNumItems()		
	{return MqReadGetNumItems(&context);}
      inline void ReadUndo() throw(MqCException)	
	{return ErrorCheck (MqReadUndo(&context)); }
    /// \}

    /// \defgroup send_API Send? : Setup and Send a ccmsgque Data-Package
    /// \{

    public:
      inline void SendSTART () throw(MqCException)	  { ErrorCheck(MqSendSTART(&context)); }
      inline void SendRETURN () throw(MqCException)	  { ErrorCheck(MqSendRETURN(&context)); }
      inline void SendERROR () throw(MqCException)	  { ErrorCheck(MqSendERROR(&context)); }
      inline void SendEND (MQ_CST const token) throw(MqCException) { 
	ErrorCheck(MqSendEND(&context, token)); 
      }
      inline void SendEND_AND_WAIT (MQ_CST const token, MQ_TIME_T timeout = MQ_TIMEOUT_USER) throw(MqCException) { 
	ErrorCheck(MqSendEND_AND_WAIT(&context, token, timeout)); 
      }
      /// \sameas{MqSendEND_AND_CALLBACK}
      MQ_EXTERN void SendEND_AND_CALLBACK (MQ_CST const token, CallbackF const callback) throw(MqCException);
      /// \sameas{MqSendEND_AND_CALLBACK}
      MQ_EXTERN void SendEND_AND_CALLBACK (MQ_CST const token, IService * const callback) throw(MqCException);

      inline void SendY (MQ_BYT val) throw(MqCException) { ErrorCheck (MqSendY (&context, val)); }
      inline void SendO (MQ_BOL val) throw(MqCException) { ErrorCheck (MqSendO (&context, val)); }
      inline void SendS (MQ_SRT val) throw(MqCException) { ErrorCheck (MqSendS (&context, val)); }
      inline void SendI (MQ_INT val) throw(MqCException) { ErrorCheck (MqSendI (&context, val)); }
      inline void SendF (MQ_FLT val) throw(MqCException) { ErrorCheck (MqSendF (&context, val)); }
      inline void SendW (MQ_WID val) throw(MqCException) { ErrorCheck (MqSendW (&context, val)); }
      inline void SendD (MQ_DBL val) throw(MqCException) { ErrorCheck (MqSendD (&context, val)); }
      inline void SendC (MQ_CST val) throw(MqCException) { ErrorCheck (MqSendC (&context, val)); }
      inline void SendU (MQ_BUF val) throw(MqCException) { ErrorCheck (MqSendU (&context, val)); }
      inline void SendN (MQ_BIN val, MQ_SIZE len) throw(MqCException) { ErrorCheck (MqSendN (&context, val, len)); }
      inline void SendBDY (MQ_BIN val, MQ_SIZE len) throw(MqCException) { ErrorCheck (MqSendBDY (&context, val, len)); }
      inline void SendB (MQ_BIN val, MQ_SIZE len) throw(MqCException) { ErrorCheck (MqSendB (&context, val, len)); }
      inline void SendB (vector<MQ_BINB>* val) throw(MqCException) { 
	ErrorCheck (MqSendB (&context, &(*val->begin()) , (int) val->size())); 
      }
      inline void SendL_START() throw(MqCException)      { ErrorCheck (MqSendL_START(&context)); }
      inline void SendL_END() throw(MqCException)	 { ErrorCheck (MqSendL_END(&context)); }
    /// \}

    /// \defgroup service_API Service? : Create and Delete a Service
    /// \{
    public:
      /// \sameas{MqServiceCreate}
      MQ_EXTERN void ServiceCreate(MQ_CST const token, CallbackF const callback) throw(MqCException);

      /// \sameas{MqServiceCreate}
      MQ_EXTERN void ServiceCreate(MQ_CST const token, IService * const service) throw(MqCException);

      /// \sameas{MqServiceDelete}
      MQ_EXTERN void ServiceDelete(MQ_CST const token) throw(MqCException);

      /// \sameas{MqServiceProxy}
      inline void ServiceProxy (MQ_CST const token, MQ_SIZE id=0) throw(MqCException) { 
	ErrorCheck (MqServiceProxy (&context, token, id));
      }
    /// \}

    /// \defgroup slave_API Slave? : Create/Get and Delete a Slave
    /// \{

    public:
      MqC *SlaveGet (const int id) { 
	struct MqS * const slave = MqSlaveGet(&context,id);
	return slave ? GetThis(slave) : NULL;
      }

      inline void SlaveCreate (const int id, MqC * const slave) throw(MqCException) {
	enum MqErrorE ret = MqSlaveCreate(&context,id,&slave->context);
	ErrorCheck (ret);
      }

      /// \sameas{MqSlaveWorker}
      void SlaveWorker (const int id, struct MqBufferLS * args = NULL) {
	ErrorCheck (MqSlaveWorker (&context, id, &args));
      }

      inline void SlaveWorkerVC (const int id, int const argc, MQ_CST *argv) {
	SlaveWorker (id, MqBufferLCreateArgsVC(&context, argc, argv));
      }

      inline void SlaveWorkerVA (const int id, va_list ap) {
	SlaveWorker (id, MqBufferLCreateArgsVA(&context, ap));
      }

      MQ_EXTERN void SlaveWorkerV (const int id, ...);

      inline void SlaveWorkerVT (const int id, vector<MQ_CST>& args) {
	SlaveWorkerVC (id, (const int) args.size(), (MQ_CST*)&(*args.begin()));
      }

      inline void SlaveDelete (const int id) throw(MqCException) { 
	ErrorCheck (MqSlaveDelete(&context,id)); 
      }
    /// \}
  };

};  // END - namespace "ccmsgque"

#endif /* MQ_CCMSGQUE_H */


