/**
 *  \file       theLink/gomsgque/src/server.go
 *  \brief      \$Id$
 *  
 *  (C) 2010 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev$
 *  \author     EMail: aotto1968 at users.berlios.de
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

package main

import (
  . "gomsgque"
    "os"
    "syscall"
)

/*
type Server struct {
  MqS
}

func NewServer() *Server {
  ret := new(Server)
  ret.Init()
  ret.ConfigSetServerSetup(ret)
  return ret
}
*/

type Server struct {
  buf *MqBufferS
}

func NewServer() *MqS {
  ctx := NewMqS()
  srv := new(Server)
  ctx.ConfigSetServerSetup(srv)
  return ctx
}

func (this *Server) ServerSetup(ctx *MqS) {
  ctx.ServiceCreate("ERR2", (*Error)(this))
  ctx.ServiceCreate("ERR3", (*Error)(this))
  ctx.ServiceCreate("ERR4", (*Error)(this))
  ctx.ServiceCreate("ERRT", (*ERRT)(this))

  ctx.ServiceCreate("CFG1", (*CFG1)(this))

  ctx.ServiceCreate("BUF1", (*BUF1)(this))
  ctx.ServiceCreate("BUF2", (*BUF2)(this))
  ctx.ServiceCreate("BUF3", (*BUF3)(this))

  ctx.ServiceCreate("SETU", (*SETU)(this))
  ctx.ServiceCreate("GETU", (*GETU)(this))

  ctx.ServiceCreate("ECOO", (*ECOO)(this))
  ctx.ServiceCreate("ECOY", (*ECOY)(this))
  ctx.ServiceCreate("ECOS", (*ECOS)(this))
  ctx.ServiceCreate("ECOI", (*ECOI)(this))
  ctx.ServiceCreate("ECOW", (*ECOW)(this))
  ctx.ServiceCreate("ECOF", (*ECOF)(this))
  ctx.ServiceCreate("ECOD", (*ECOD)(this))

  ctx.ServiceCreate("ECOU", (*ECOU)(this))
  ctx.ServiceCreate("ECOB", (*ECOB)(this))
  ctx.ServiceCreate("ECUL", (*ECUL)(this))

  ctx.ServiceCreate("ECOL", (*ECOL)(this))
  ctx.ServiceCreate("ECLI", (*ECLI)(this))
  ctx.ServiceCreate("LST1", (*LST1)(this))
  ctx.ServiceCreate("LST2", (*LST2)(this))
  ctx.ServiceCreate("ERLS", (*ERLS)(this))
  ctx.ServiceCreate("ERLR", (*ERLR)(this))
}

type ERLR Server
  func (this *ERLR) Call(ctx *MqS) {
    ctx.SendSTART();
    ctx.ReadL_START(nil);
    ctx.ReadL_START(nil);
    ctx.SendRETURN();
  }

type ERLS Server
  func (this *ERLS) Call(ctx *MqS) {
    ctx.SendSTART();
    ctx.SendL_START();
    ctx.SendU(ctx.ReadU());
    ctx.SendL_START();
    ctx.SendU(ctx.ReadU());
    ctx.SendRETURN();
  }

type LST1 Server
  func (this *LST1) Call(ctx *MqS) {
    ctx.SendSTART();
    ctx.SendL_END();
    ctx.SendRETURN();
  }

type LST2 Server
  func (this *LST2) Call(ctx *MqS) {
    ctx.SendSTART();
    ctx.ReadL_END();
    ctx.SendRETURN();
  }

  func (this *Server) EchoList(ctx *MqS, doincr bool) {
    for ctx.ReadItemExists() {
      buf := ctx.ReadU()
      if (buf.GetType() == MQ_LSTT) {
	ctx.ReadL_START(buf)
	ctx.SendL_START()
	this.EchoList(ctx, doincr)
	ctx.SendL_END()
	ctx.ReadL_END()
      } else if (doincr) {
	ctx.SendI (buf.GetI()+1)
      } else {
	ctx.SendU(buf)
      }
    }
  }

type ECOL Server
  func (this *ECOL) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.ReadL_START(nil)
    ctx.SendL_START()
    (*Server)(this).EchoList(ctx, false)
    ctx.SendL_END()
    ctx.ReadL_END()
    ctx.SendRETURN()
  }

type ECLI Server
  func (this *ECLI) Call(ctx *MqS) {
    opt := ctx.ReadU()
    doincr := (opt.GetType() == MQ_STRT && opt.GetC() == "--incr")
    if (!doincr) { ctx.ReadUndo() }
    ctx.SendSTART()
    (*Server)(this).EchoList(ctx, doincr)
    ctx.SendRETURN()
  }

type ERRT Server
  func (this *ERRT) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.ErrorC("MYERR", 9, ctx.ReadC())
    ctx.SendERROR()
  }

type Error Server
  func (this *Error) Call(ctx *MqS) {
    ctx.SendSTART()
    switch ctx.ServiceGetToken() {
      case "ERR2" : {
	ctx.SendC("some data")
	ctx.ErrorC("Ot_ERR2", 10, "some error");
      }
      case "ERR3" : ctx.SendRETURN()
      case "ERR4" : syscall.Exit(1)
    }
    ctx.SendRETURN()
  }

type CFG1 Server
  func (this *CFG1) Call(ctx *MqS) {
    ctx.SendSTART()
    switch ctx.ReadC() {
      case "Buffersize": {
	old := ctx.ConfigGetBuffersize()
	ctx.ConfigSetBuffersize (ctx.ReadI())
	ctx.SendI (ctx.ConfigGetBuffersize())
	ctx.ConfigSetBuffersize (old)
      }
      case "Debug": {
	old := ctx.ConfigGetDebug()
	ctx.ConfigSetDebug (ctx.ReadI())
	ctx.SendI (ctx.ConfigGetDebug())
	ctx.ConfigSetDebug (old)
      }
      case "Timeout": {
	old := ctx.ConfigGetTimeout()
	ctx.ConfigSetTimeout (ctx.ReadW())
	ctx.SendW (ctx.ConfigGetTimeout())
	ctx.ConfigSetTimeout (old)
      }
      case "Name": {
	old := ctx.ConfigGetName()
	ctx.ConfigSetName (ctx.ReadC())
	ctx.SendC (ctx.ConfigGetName())
	ctx.ConfigSetName (old)
      }
      case "SrvName": {
	old := ctx.ConfigGetSrvName()
	ctx.ConfigSetSrvName (ctx.ReadC())
	ctx.SendC (ctx.ConfigGetSrvName())
	ctx.ConfigSetSrvName (old)
      }
      case "Ident": {
	old := ctx.ConfigGetIdent()
	ctx.ConfigSetIdent (ctx.ReadC())
	check := ctx.LinkGetTargetIdent() == ctx.ReadC()
	ctx.SendSTART()
	ctx.SendC (ctx.ConfigGetIdent())
	ctx.SendO (check);
	ctx.ConfigSetIdent (old)
      }
      case "IsSilent": {
	old := ctx.ConfigGetIsSilent()
	ctx.ConfigSetIsSilent (ctx.ReadO())
	ctx.SendO (ctx.ConfigGetIsSilent())
	ctx.ConfigSetIsSilent (old)
      }
      case "IsString": {
	old := ctx.ConfigGetIsString()
	ctx.ConfigSetIsString (ctx.ReadO())
	ctx.SendO (ctx.ConfigGetIsString())
	ctx.ConfigSetIsString (old)
      }
      case "IoUds": {
	old := ctx.ConfigGetIoUdsFile()
	ctx.ConfigSetIoUdsFile (ctx.ReadC())
	ctx.SendC (ctx.ConfigGetIoUdsFile())
	ctx.ConfigSetIoUdsFile (old)
      }
      case "IoTcp": {
	h  := ctx.ConfigGetIoTcpHost   ()
	p  := ctx.ConfigGetIoTcpPort   ()
	mh := ctx.ConfigGetIoTcpMyHost ()
	mp := ctx.ConfigGetIoTcpMyPort ()
	hv := ctx.ReadC()
	pv := ctx.ReadC()
	mhv := ctx.ReadC()
	mpv := ctx.ReadC()
	ctx.ConfigSetIoTcp (hv,pv,mhv,mpv)
	ctx.SendC (ctx.ConfigGetIoTcpHost())
	ctx.SendC (ctx.ConfigGetIoTcpPort())
	ctx.SendC (ctx.ConfigGetIoTcpMyHost())
	ctx.SendC (ctx.ConfigGetIoTcpMyPort())
	ctx.ConfigSetIoTcp (h,p,mh,mp)
      }
      case "IoPipe": {
	old := ctx.ConfigGetIoPipeSocket()
	ctx.ConfigSetIoPipeSocket (ctx.ReadI())
	ctx.SendI (ctx.ConfigGetIoPipeSocket())
	ctx.ConfigSetIoPipeSocket (old)
      }
      case "StartAs": {
	old := ctx.ConfigGetStartAs()
	ctx.ConfigSetStartAs (MqStartE(ctx.ReadI()))
	ctx.SendI (int32(ctx.ConfigGetStartAs()))
	ctx.ConfigSetStartAs (old)
      }
      default: {
	ctx.ErrorC ("CFG1", 1, "invalid command: ")
      }
    }
    ctx.SendRETURN();
  }

type BUF3 Server
  func (this *BUF3) Call(ctx *MqS) {
    ctx.SendSTART()
    buf := ctx.ReadU()
    ctx.SendC(buf.GetTypeC())
    ctx.SendU(buf)
    ctx.SendI(ctx.ReadI())
    ctx.SendU(buf)
    ctx.SendRETURN()
  }

type BUF2 Server
  func (this *BUF2) Call(ctx *MqS) {
    ctx.SendSTART()
    for i:=0; i<3; i++ {
      buf := ctx.ReadU()
      ctx.SendC(buf.GetTypeC())
      ctx.SendU(buf)
    }
    ctx.SendRETURN()
  }

type BUF1 Server
  func (this *BUF1) Call(ctx *MqS) {
    buf := ctx.ReadU()
    ctx.SendSTART()
    ctx.SendC(buf.GetTypeC())
    switch (buf.GetType()) {
      case MQ_BYTT: ctx.SendY(buf.GetY())
      case MQ_BOLT: ctx.SendO(buf.GetO())
      case MQ_SRTT: ctx.SendS(buf.GetS())
      case MQ_INTT: ctx.SendI(buf.GetI())
      case MQ_FLTT: ctx.SendF(buf.GetF())
      case MQ_WIDT: ctx.SendW(buf.GetW())
      case MQ_DBLT: ctx.SendD(buf.GetD())
      case MQ_STRT: ctx.SendC(buf.GetC())
      case MQ_BINT: ctx.SendB(buf.GetB())
    }
    ctx.SendRETURN()
  }

type ECOB Server
  func (this *ECOB) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendB(ctx.ReadB())
    ctx.SendRETURN()
  }

type ECOD Server
  func (this *ECOD) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendD(ctx.ReadD())
    ctx.SendRETURN()
  }

type ECOF Server
  func (this *ECOF) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendF(ctx.ReadF())
    ctx.SendRETURN()
  }

type SETU Server
  func (this *SETU) Call(ctx *MqS) {
    this.buf = ctx.ReadU()
  }

type GETU Server
  func (this *GETU) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendU(this.buf)
    ctx.SendRETURN()
  }

type ECOO Server
  func (this *ECOO) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendO(ctx.ReadO())
    ctx.SendRETURN()
  }

type ECOY Server
  func (this *ECOY) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendY(ctx.ReadY())
    ctx.SendRETURN()
  }

type ECOS Server
  func (this *ECOS) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendS(ctx.ReadS())
    ctx.SendRETURN()
  }

type ECOI Server
  func (this *ECOI) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendI(ctx.ReadI())
    ctx.SendRETURN()
  }

type ECOW Server
  func (this *ECOW) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendW(ctx.ReadW())
    ctx.SendRETURN()
  }

type ECOU Server
  func (this *ECOU) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendU(ctx.ReadU())
    ctx.SendRETURN()
  }

type ECUL Server
  func (this *ECUL) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendY(ctx.ReadY())
    ctx.SendS(ctx.ReadS())
    ctx.SendI(ctx.ReadI())
    ctx.SendW(ctx.ReadW())
    ctx.ReadProxy(ctx)
    ctx.SendRETURN()
    //ctx.ErrorC("ECUL",-1,"fehler")
  }

func main() {
  var srv = NewServer()
  defer func() {
    if x := recover(); x != nil {
      srv.ErrorSet(x)
    }
    srv.Exit()
  }()
  srv.ConfigSetName("server")
  srv.ConfigSetIdent("test-server")
  srv.LinkCreate(os.Args...)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS_TIMEOUT_DEFAULT, MqS_WAIT_FOREVER)
}

