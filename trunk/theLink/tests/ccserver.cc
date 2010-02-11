/**
 *  \file       theLink/tests/ccserver.cc
 *  \brief      \$Id$
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <fstream>

#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

namespace example {

  class Client : public MqC, public IService, public IFactory, public IBgError {

    private:
      virtual void Service (MqC * const ctx) {
	i = ctx->ReadI();
      }
      virtual MqC* Factory () const  {
	return new Client();
      }
      virtual void BgError () {
	MqC *master = SlaveGetMaster();
	if (master != NULL) {
	  master->ErrorC ("BGERROR", ErrorGetNum(), ErrorGetText());
	  master->SendERROR();
	}
      }

    public:
      int i;
    
      void LinkCreate (MQ_INT debug) {
	ConfigSetDebug   (debug);
	MqC::LinkCreateV ("test-client", "@", "SELF", "--name", "test-server", NULL); 
      }

      void LinkCreateChild (Client& parent) {
	MqC::LinkCreateChild(static_cast<MqC&>(parent), NULL); 
      }

      Client () : MqC(), i(0) { }
  };

  class ClientERR : public MqC, public IService {

    private:
      virtual void Service (MqC * const ctx) {
	i = ctx->ReadI();
      }

    public:
      int i;
    
      void LinkCreate (int debug) throw(MqCException) {
	ConfigSetDebug (debug);
	MqC::LinkCreateV("test-client", "@", "SELF", NULL); 
      }

      void LinkCreateChild (ClientERR& parent) {
	MqC::LinkCreateChild(static_cast<MqC&>(parent), NULL); 
      }

      ClientERR () : MqC(), i(0) {
	ConfigSetName ("test-client");
	ConfigSetSrvName ("test-server");
      }
  };

  class ClientERR2 : public MqC {

    public:
      int i;
    
      void LinkCreate (int debug) throw(MqCException) {
	ConfigSetDebug (debug);
	MqC::LinkCreateV("test-client", "@", "DUMMY", NULL); 
      }

      ClientERR2 () : MqC(), i(0) {
	ConfigSetName ("cl-err-1");
      }
  };

  class Server : public MqC, public IService, public IServerSetup, public IServerCleanup, public IFactory {

    private:
      MQ_INT myInt;
      MQ_BUF buf;
      Client* cl[3];

    public:
      Server() : MqC(), myInt(0), buf(NULL) {
	int i=0;
	for (i=0; i<3; i++) {
	  cl[i] = NULL;
	}	
      }
      ~Server() { }

    private:

      void ECOY () { SendSTART(); SendY(ReadY()); SendRETURN(); }
      void ECOO () { SendSTART(); SendO(ReadO()); SendRETURN(); }
      void ECOS () { SendSTART(); SendS(ReadS()); SendRETURN(); }
      void ECOI () { SendSTART(); SendI(ReadI()); SendRETURN(); }
      void ECOF () { SendSTART(); SendF(ReadF()); SendRETURN(); }
      void ECOW () { SendSTART(); SendW(ReadW()); SendRETURN(); }
      void ECOD () { SendSTART(); SendD(ReadD()); SendRETURN(); }
      void ECOC () { SendSTART(); SendC(ReadC()); SendRETURN(); }
      void ECOB () { 
	MQ_BIN bin;
	MQ_SIZE len;
	SendSTART(); 
	ReadB(&bin, &len);
	SendB(bin, len); 
	SendRETURN(); 
      }
      void ECOU () { SendSTART(); ReadProxy(this); SendRETURN(); }
      void GTTO () { SendSTART(); SendC(ServiceGetToken()); SendRETURN(); }

      void ECON () { 
	string str = ReadC();
	str.append("-");
	str.append(ConfigGetName());
	SendSTART(); 
	SendC(str.data()); 
	SendRETURN(); 
      }

      void ERR2() {
	SendSTART(); 
	SendC ("some data");
	ErrorC ("Ot_ERR2", 10, "some error");
	SendRETURN(); 
      }

      void ERR3() {
	SendSTART(); 
	SendRETURN(); 
	SendRETURN(); 
      }

      void ERR4() {
	exit (1);
      }

      void SETU () { buf = ReadU(); }
      void GETU () { SendSTART(); SendU(buf); SendRETURN(); }

      void ECOL_Item (int incr) {
	while (ReadItemExists()) {
	  MqBufferC buf = ReadU();
	  if (buf.GetType() == MQ_LSTT) {
	    ReadL_START (buf);
	    SendL_START ();
	    ECOL_Item	(incr);
	    SendL_END	();
	    ReadL_END	();
	  } else if (incr) {
	    SendI (buf.GetI() + 1);
	  } else {
	    SendU (buf.GetU());
	  }
	}
      }

      void ECOL() {
	SendSTART(); 
	ReadL_START ();
	SendL_START ();
	ECOL_Item   (0);
	SendL_END   ();
	ReadL_END   ();
	SendRETURN(); 
      }

      void ECLI() {
	MQ_BOL doincr;

	MqBufferC buf = MqBufferC(ReadU());
	doincr = (buf.GetType() == MQ_STRT && strncmp(buf.GetC(),"--incr",6) == 0);
	if (!doincr) ReadUndo();

	SendSTART(); 
	ECOL_Item (doincr);
	SendRETURN(); 
      }

      void LST1() {
	SendSTART   (); 
	SendL_END   ();
	SendRETURN  (); 
      }

      void LST2() {
	SendSTART   (); 
	ReadL_END   ();
	SendRETURN  (); 
      }

      void INIT() {
	struct MqBufferLS * initB = MqInitCreate();
	SendSTART   (); 
	while (ReadItemExists()) {
	  MqBufferLAppendC(initB, ReadC());
	}
	SendRETURN  (); 
      }

      void SND1() {
	MQ_CST s= ReadC() ;
	MQ_INT id = ReadI();

	SendSTART();
	  if (!strcmp(s,"START")) {
	    Server *parent = static_cast<Server*>(LinkGetParent());
	    if (parent != NULL && parent->cl[id]->LinkIsConnected()) {
	      cl[id]->LinkCreateChild(*parent->cl[id]);
	    } else {
	      cl[id]->LinkCreate(ConfigGetDebug());
	    }
	  } else if (!strcmp(s,"START2")) {
	    // object already connected ERROR
	    cl[id]->LinkCreate(ConfigGetDebug());
	    cl[id]->LinkCreate(ConfigGetDebug());
	  } else if (!strcmp(s,"START3")) {
	    Client parent;
	    cl[id]->LinkCreateChild(parent);
	  } else if (!strcmp(s,"START4")) {
	    // master not connected ERROR
	    cl[id]->SlaveWorker(0);
	  } else if (!strcmp(s,"START5")) {
	    // the 'master' have to be a 'parent' without 'child' objects
	    // 'slave' identifer out of range (0 <= 10000000 <= 1023)
	    char cl[13], sv[13];
	    sprintf(cl,"wk-cl-%d",id);
	    sprintf(sv,"wk-sv-%d",id);
	    SlaveWorkerV(id, "--name", cl, "--srvname", sv, "--thread", NULL);
	  } else if (!strcmp(s,"STOP")) {
	    cl[id]->LinkDelete();
	  } else if (!strcmp(s,"SEND")) {
	    MQ_CST TOK = ReadC();
	    cl[id]->SendSTART();
	    ReadProxy(cl[id]);
	    cl[id]->SendEND(TOK);
	  } else if (!strcmp(s,"WAIT")) {
	    cl[id]->SendSTART();
	    ReadProxy(cl[id]);
	    cl[id]->SendEND_AND_WAIT("ECOI", 5);
	    SendI(cl[id]->ReadI()+1);
	  } else if (!strcmp(s,"CALLBACK")) {
	    cl[id]->i = -1;
	    cl[id]->SendSTART();
	    ReadProxy(cl[id]);
	    cl[id]->SendEND_AND_CALLBACK("ECOI", static_cast<IService*>(cl[id]));
	    cl[id]->ProcessEvent(10, MQ_WAIT_ONCE);
	    SendI(cl[id]->i+1);
	  } else if (!strcmp(s,"ERR-1")) {
	    cl[id]->SendSTART();
	    try { 
	      ReadProxy(cl[id]);
	      cl[id]->SendEND_AND_WAIT("ECOI", 5);
	    } catch (const exception& e) {
	      ErrorSet (e);
	      SendI (ErrorGetNum());
	      SendC (ErrorGetText());
	      ErrorReset();
	    }
	  }
	SendRETURN ();
      }

      void GTCX () {
	SendSTART();
	SendI(LinkGetCtxId());
	SendRETURN();
      }

      void REDI () {
	ReadI();
      }

      void CSV1 () {

	// call an other service
	SendSTART ();
	SendI (ReadI()+1);
	SendEND_AND_WAIT ("CSV2", 10);

	// read the answer and send the result back
	SendSTART ();
	SendI (ReadI()+1);
	SendRETURN ();
      }

      void SLEP () {
	int i;
	SendSTART();
	i = ReadI();
	Sleep (i);
	SendI (i);
	SendRETURN();
      }

      void USLP () {
	SendSTART();
	int i = ReadI();
	USleep (i);
	SendI  (i);
	SendRETURN();
      }

      void MSQT () {
	SendSTART();
	if (ConfigGetDebug() != 0) {
	  SendC ("debug");
	  SendI (ConfigGetDebug());
	}
	if (ConfigGetIsString() == false)
	  SendC ("binary");
	if (ConfigGetIsSilent() == true)
	  SendC ("silent");
	SendC ("sOc");
	SendC (ConfigGetIsServer() ? "SERVER" : "CLIENT");
	SendC ("pOc");
	SendC (LinkIsParent() ? "PARENT" : "CHILD");
	SendRETURN();
      }

      void BUF1 () {
	MqBufferC buf = ReadU();
	const char typ[2] = { MqBufferGetType(buf.GetU()), '\0' };
	SendSTART();
	SendC(typ);
	switch (buf.GetType()) {
	  case MQ_BYTT: SendY(buf.GetY()); break;
	  case MQ_BOLT: SendO(buf.GetO()); break;
	  case MQ_SRTT: SendS(buf.GetS()); break;
	  case MQ_INTT: SendI(buf.GetI()); break;
	  case MQ_FLTT: SendF(buf.GetF()); break;
	  case MQ_WIDT: SendW(buf.GetW()); break;
	  case MQ_DBLT: SendD(buf.GetD()); break;
	  case MQ_STRT: SendC(buf.GetC()); break;
	  case MQ_BINT: {
	    MQ_BIN b; MQ_SIZE len;
	    buf.GetB(&b, &len);
	    SendB(b, len); 
	    break;
	  }
	  case MQ_LSTT: break;
	  case MQ_RETT: break;
	}
	SendRETURN();
      }

      void BUF2 () {
	SendSTART();
	for (int i=0; i<3; i++) {
	  MQ_BUF buf = ReadU();
	  const char str[2] = {MqBufferGetType(buf), '\0'};
	  SendC(str);
	  SendU(buf);
	}
	SendRETURN();
      }

      void BUF3 () {
	MQ_BUF buf = ReadU();
	SendSTART();
	const char str[2] = {MqBufferGetType(buf), '\0'};
	SendC(str);
	SendU(buf);
	SendI(ReadI());
	SendU(buf);
	SendRETURN();
      }

      void Service(MqC * const ctx) {
	myInt = ctx->ReadI();
      }

      void SND2 () {
	MQ_CST s = ReadC();
	MQ_INT id = ReadI();
	Server *cl = static_cast<Server*>(SlaveGet(id));

	SendSTART();
	  if (!strcmp(s,"CREATE")) {
	    char cl[10], sv[10];
	    sprintf(cl, "wk-cl-%d", id);
	    sprintf(sv, "wk-sv-%d", id);
	    vector<MQ_CST> LIST;
	    while (ReadItemExists())
	      LIST.push_back(ReadC());
	    LIST.push_back("--name");
	    LIST.push_back(cl);
	    LIST.push_back("@");
	    LIST.push_back("--name");
	    LIST.push_back(sv);
	    SlaveWorkerVT (id, LIST);
	  } else if (!strcmp(s,"CREATE2")) {
	    Client *slv = new Client();
	    slv->LinkCreate(ConfigGetDebug());
	    SlaveCreate (id, slv);
	  } else if (!strcmp(s,"CREATE3")) {
	    ClientERR *slv = new ClientERR();
	    slv->LinkCreate(ConfigGetDebug());
	    SlaveCreate (id, slv);
	  } else if (!strcmp(s,"DELETE")) {
	    SlaveDelete(id);
	    SendC(SlaveGet(id) == NULL ? "OK" : "ERROR");
	  } else if (!strcmp(s,"SEND")) {
	    cl->SendSTART();
	    MQ_CST TOK = ReadC();
	    ReadProxy(cl);
	    cl->SendEND(TOK);
	  } else if (!strcmp(s,"WAIT")) {
	    MQ_BIN itm;
	    MQ_SIZE len;
	    cl->SendSTART();
	    ReadN(&itm, &len);
	    cl->SendN(itm, len);
	    cl->SendEND_AND_WAIT("ECOI", 5);
	    SendI(cl->ReadI()+1);
	  } else if (!strcmp(s,"CALLBACK")) {
	    cl->SendSTART();
	    ReadProxy(cl);
	    myInt = -1;
	    cl->SendEND_AND_CALLBACK("ECOI", static_cast<IService* const>(this));
	    cl->ProcessEvent(10, MQ_WAIT_ONCE);
	    SendI(myInt+1);
	  } else if (!strcmp(s,"MqSendEND_AND_WAIT")) {
	    MQ_CST TOK = ReadC();
	    cl->SendSTART();
	    while (ReadItemExists())
	      ReadProxy(cl);
	    cl->SendEND_AND_WAIT(TOK, 5);
	    while (cl->ReadItemExists())
	      cl->ReadProxy(this);
	  } else if (!strcmp(s,"MqSendEND")) {
	    MQ_CST TOK = ReadC();
	    cl->SendSTART();
	    while (ReadItemExists())
	      ReadProxy(cl);
	    cl->SendEND(TOK);
	    return;
	  } else if (!strcmp(s,"ERR-1")) {
	    ClientERR2 *slv = new ClientERR2();
	    slv->LinkCreate(ConfigGetDebug());
	  } else if (!strcmp(s,"isSlave")) {
	    SendO(cl->SlaveIs());
	  }
	SendRETURN();
      }

      void ECUL () {
	SendSTART();
	SendY(ReadY());
	SendS(ReadS());
	SendI(ReadI());
	SendW(ReadW());
	ReadProxy(this);
	SendRETURN();
      }

      void RDUL () {
	ReadY();
	ReadS();
	ReadI();
	ReadW();
	ReadU();
      }

      void CNFG () {
	SendSTART();
	SendO(ConfigGetIsServer());
	SendO(LinkIsParent());
	SendO(SlaveIs());
	SendO(ConfigGetIsString());
	SendO(ConfigGetIsSilent());
	SendO(LinkIsConnected());
	SendC(ConfigGetName());
	SendI(ConfigGetDebug());
	SendI(LinkGetCtxId());
	SendC(ServiceGetToken());
	SendRETURN();
      }

      void ERLR () {
	SendSTART();
	ReadL_START();
	ReadL_START();
	SendRETURN();
      }

      void ERLS () {
	SendSTART();
	SendL_START();
	SendU(ReadU());
	SendL_START();
	SendU(ReadU());
	SendRETURN();
      }

      void CFG1 () {
	MQ_CST cmd = ReadC();

	SendSTART();

	if (!strncmp(cmd, "Buffersize", 10)) {
	  MQ_INT old = ConfigGetBuffersize();
	  ConfigSetBuffersize (ReadI());
	  SendI (ConfigGetBuffersize());
	  ConfigSetBuffersize (old);
	} else if (!strncmp(cmd, "Debug", 5)) {
	  MQ_INT old = ConfigGetDebug();
	  ConfigSetDebug (ReadI());
	  SendI (ConfigGetDebug());
	  ConfigSetDebug (old);
	} else if (!strncmp(cmd, "Timeout", 7)) {
	  MQ_WID old (ConfigGetTimeout());
	  ConfigSetTimeout (ReadW());
	  SendW (ConfigGetTimeout());
	  ConfigSetTimeout (old);
	} else if (!strncmp(cmd, "Name", 4)) {
	  MQ_CST old = mq_strdup_save(ConfigGetName());
	  ConfigSetName (ReadC());
	  SendC (ConfigGetName());
	  ConfigSetName (old);
	  MqSysFree(old);
	} else if (!strncmp(cmd, "SrvName", 7)) {
	  MQ_CST old = mq_strdup_save(ConfigGetSrvName());
	  ConfigSetSrvName (ReadC());
	  SendC (ConfigGetSrvName());
	  ConfigSetSrvName (old);
	  MqSysFree(old);
	} else if (!strncmp(cmd, "Ident", 5)) {
	  MQ_CST old = mq_strdup_save(ConfigGetSrvName());
	  MQ_BOL check;
	  ConfigSetSrvName (ReadC());
	  check = !strcmp(LinkGetTargetIdent(), ReadC());
	  SendSTART();
	  SendC (ConfigGetSrvName());
	  SendO (check);
	  ConfigSetSrvName (old);
	  MqSysFree(old);
	} else if (!strncmp(cmd, "IsSilent", 8)) {
	  MQ_BOL old = ConfigGetIsSilent();
	  ConfigSetIsSilent (ReadO());
	  SendO (ConfigGetIsSilent());
	  ConfigSetIsSilent (old);
	} else if (!strncmp(cmd, "IsString", 8)) {
	  MQ_BOL old = ConfigGetIsString();
	  ConfigSetIsString (ReadO());
	  SendO (ConfigGetIsString());
	  ConfigSetIsString (old);
	} else if (!strncmp(cmd, "IoUds", 5)) {
	  MQ_CST old = mq_strdup_save(ConfigGetIoUdsFile());
	  ConfigSetIoUds (ReadC());
	  SendC (ConfigGetIoUdsFile());
	  ConfigSetIoUds (old);
	  MqSysFree(old);
	} else if (!strncmp(cmd, "IoTcp", 5)) {
	  MQ_CST h,p,mh,mp;
	  MQ_CST hv,pv,mhv,mpv;
	  h  = mq_strdup_save(ConfigGetIoTcpHost ());
	  p  = mq_strdup_save(ConfigGetIoTcpPort ());
	  mh = mq_strdup_save(ConfigGetIoTcpMyHost ());
	  mp = mq_strdup_save(ConfigGetIoTcpMyPort ());
	  hv = ReadC();
	  pv = ReadC();
	  mhv = ReadC();
	  mpv = ReadC();
	  ConfigSetIoTcp (hv,pv,mhv,mpv);
	  SendC (ConfigGetIoTcpHost());
	  SendC (ConfigGetIoTcpPort());
	  SendC (ConfigGetIoTcpMyHost());
	  SendC (ConfigGetIoTcpMyPort());
	  ConfigSetIoTcp (h,p,mh,mp);
	  MqSysFree(h);
	  MqSysFree(p);
	  MqSysFree(mh);
	  MqSysFree(mp);
	} else if (!strncmp(cmd, "IoPipe", 6)) {
	  MQ_SOCK old = ConfigGetIoPipeSocket();
	  ConfigSetIoPipe (ReadI());
	  SendI (ConfigGetIoPipeSocket());
	  ConfigSetIoPipe (old);
	} else if (!strncmp(cmd, "StartAs", 5)) {
	  MQ_INT old = ConfigGetStartAs();
	  ConfigSetStartAs ((enum MqStartE)ReadI());
	  SendI (ConfigGetStartAs());
	  ConfigSetStartAs ((enum MqStartE)old);
	} else {
	  ErrorV ("CFG1", 1, "invalid command: %s", cmd);
	}
	SendRETURN();
      }

      void PRNT () {
	SendSTART();
	SendV("%d - %s", LinkGetCtxId(), ReadC());
	SendEND_AND_WAIT("WRIT");
	SendRETURN();
      }

    private:
      // factory  used to create server object instances
      MqC* Factory() const { return new Server(); }

      void ServerCleanup() {
	// Cleanup context variables
	int i=0;
	for (i=0; i<3; i++) {
	  if (cl[i] == NULL) continue;
	  delete cl[i];
	  cl[i] = NULL;
	}	
      }

      void ServerSetup() {
	if (SlaveIs()) {
	  // add "slave" services here
	} else {
	  MQ_BUF buf = GetTempBuffer();
	  // Initialize context variables
	  int i=0;
	  for (i=0; i<3; i++) {
	    cl[i] = new Client();
	    MqBufferSetV(buf, "cl-%i", i);
	    cl[i]->ConfigSetName(buf->cur.C);
	    MqBufferSetV(buf, "sv-%i", i);
	    cl[i]->ConfigSetSrvName(buf->cur.C);
	  }	
	  // add "master" services here
	  ServiceCreate("ECOY", CallbackF(&Server::ECOY));
	  ServiceCreate("ECOO", CallbackF(&Server::ECOO));
	  ServiceCreate("ECOS", CallbackF(&Server::ECOS));
	  ServiceCreate("ECOI", CallbackF(&Server::ECOI));
	  ServiceCreate("ECOF", CallbackF(&Server::ECOF));
	  ServiceCreate("ECOW", CallbackF(&Server::ECOW));
	  ServiceCreate("ECOD", CallbackF(&Server::ECOD));
	  ServiceCreate("ECOC", CallbackF(&Server::ECOC));
	  ServiceCreate("ECOB", CallbackF(&Server::ECOB));
	  ServiceCreate("ECOU", CallbackF(&Server::ECOU));
	  ServiceCreate("SETU", CallbackF(&Server::SETU));
	  ServiceCreate("GETU", CallbackF(&Server::GETU));
	  ServiceCreate("GTTO", CallbackF(&Server::GTTO));
	  ServiceCreate("ECON", CallbackF(&Server::ECON));
	  ServiceCreate("ERR2", CallbackF(&Server::ERR2));
	  ServiceCreate("ERR3", CallbackF(&Server::ERR3));
	  ServiceCreate("ERR4", CallbackF(&Server::ERR4));
	  ServiceCreate("ECOL", CallbackF(&Server::ECOL));
	  ServiceCreate("ECLI", CallbackF(&Server::ECLI));
	  ServiceCreate("LST1", CallbackF(&Server::LST1));
	  ServiceCreate("LST2", CallbackF(&Server::LST2));
	  ServiceCreate("INIT", CallbackF(&Server::INIT));
	  ServiceCreate("SND1", CallbackF(&Server::SND1));
	  ServiceCreate("SND2", CallbackF(&Server::SND2));
	  ServiceCreate("GTCX", CallbackF(&Server::GTCX));
	  ServiceCreate("REDI", CallbackF(&Server::REDI));
	  ServiceCreate("CSV1", CallbackF(&Server::CSV1));
	  ServiceCreate("SLEP", CallbackF(&Server::SLEP));
	  ServiceCreate("USLP", CallbackF(&Server::USLP));
	  ServiceCreate("MSQT", CallbackF(&Server::MSQT));
	  ServiceCreate("BUF1", CallbackF(&Server::BUF1));
	  ServiceCreate("BUF2", CallbackF(&Server::BUF2));
	  ServiceCreate("BUF3", CallbackF(&Server::BUF3));
	  ServiceCreate("ECUL", CallbackF(&Server::ECUL));
	  ServiceCreate("RDUL", CallbackF(&Server::RDUL));
	  ServiceCreate("CNFG", CallbackF(&Server::CNFG));
	  ServiceCreate("ERLR", CallbackF(&Server::ERLR));
	  ServiceCreate("ERLS", CallbackF(&Server::ERLS));
	  ServiceCreate("CFG1", CallbackF(&Server::CFG1));
	  ServiceCreate("PRNT", CallbackF(&Server::PRNT));
	}
      }
  };
}

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

using namespace example;

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Server server;
  try {
    server.ConfigSetName ("server");
    server.LinkCreateVC (argc, argv);
    server.LogC("test",1,"this is the log test\n");
    server.ProcessEvent (MQ_WAIT_FOREVER);  
  } catch (const exception& e) {
    // Convert the "exception" object in an "MqCException" object, "Exit" will report the error to the client
    server.ErrorSet (e);
  }
  // report error to client, shutdown the link and exit the application
  server.Exit ();
}

