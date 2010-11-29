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
    "fmt"
    "time"
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

type ClientERR struct {
  *MqS
  i int32
}

func NewClientERR() *ClientERR {
  ret := new(ClientERR)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *ClientERR) LinkCreate(debug int32) {
  this.ConfigSetDebug(debug)
  this.ConfigSetName("test-client")
  this.ConfigSetSrvName("test-server")
  this.MqS.LinkCreate(os.Args[0], "@", "SELF")
}

func (this *ClientERR) ECOI_CB(ctx *MqS) {
  this.i = ctx.ReadI()
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

type ClientERR2 struct {
  *MqS
}

func NewClientERR2() *ClientERR2 {
  ret := new(ClientERR2)
  ret.MqS = NewMqS(ret)
  return ret
}

func (this *ClientERR2) LinkCreate(debug int32) {
  this.ConfigSetDebug(debug)
  this.ConfigSetName("cl-err-1")
  this.MqS.LinkCreate(os.Args[0], "@", "DUMMY")
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

type Client struct {
  *MqS
  i int32
}

func NewClient() *Client {
  srv := new(Client)
  srv.MqS = NewMqS(srv)
  return srv
}

func (this *Client) Factory() *MqS {
  return NewClient().MqS
}

func (this *Client) BgError() {
  master := this.SlaveGetMaster()
  if (master != nil) {
    master.ErrorC ("BGERROR", this.ErrorGetNum(), this.ErrorGetText())
    master.SendERROR ()
  }
}

func (this *Client) LinkCreate(debug int32) {
  this.ConfigSetDebug(debug)
  this.MqS.LinkCreate(os.Args[0], "@", "SELF", "--name", "test-server")
}

type ECOI_CB Client
func (this *ECOI_CB) Call(ctx *MqS) {
  this.i = ctx.ReadI()
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

type Server struct {
  *MqS
  buf *MqBufferS
  cl  [3]*Client
  i   int32
  j int32
}

func NewServer() *Server {
  srv := new(Server)
  srv.MqS = NewMqS(srv)
  return srv
}

func (this *Server) Factory() *MqS {
  return NewServer().MqS
}

func (this *Server) ServerCleanup() {
  for i,cl := range this.cl {
    if cl == nil { continue }
    cl.LinkDelete()
    this.cl[i] = nil
  }
}

func (this *Server) ServerSetup() {
  if (this.SlaveIs()) {
    // add "slave" services here
  } else {
    for i := range this.cl {
      this.cl[i] = NewClient()
      this.cl[i].ConfigSetName(fmt.Sprintf("cl-%d", i))
      this.cl[i].ConfigSetSrvName(fmt.Sprintf("sv-%d", i))
    }
    // add "master" services here
    this.ServiceCreate("ERR2", (*Error)(this))
    this.ServiceCreate("ERR3", (*Error)(this))
    this.ServiceCreate("ERR4", (*Error)(this))
    this.ServiceCreate("ERRT", (*ERRT)(this))

    this.ServiceCreate("CFG1", (*CFG1)(this))

    this.ServiceCreate("BUF1", (*BUF1)(this))
    this.ServiceCreate("BUF2", (*BUF2)(this))
    this.ServiceCreate("BUF3", (*BUF3)(this))

    this.ServiceCreate("SETU", (*SETU)(this))
    this.ServiceCreate("GETU", (*GETU)(this))

    this.ServiceCreate("ECOO", (*ECOO)(this))
    this.ServiceCreate("ECOY", (*ECOY)(this))
    this.ServiceCreate("ECOS", (*ECOS)(this))
    this.ServiceCreate("ECOI", (*ECOI)(this))
    this.ServiceCreate("ECOW", (*ECOW)(this))
    this.ServiceCreate("ECOF", (*ECOF)(this))
    this.ServiceCreate("ECOD", (*ECOD)(this))
    this.ServiceCreate("ECOC", (*ECOC)(this))

    this.ServiceCreate("ECOU", (*ECOU)(this))
    this.ServiceCreate("ECOB", (*ECOB)(this))
    this.ServiceCreate("ECUL", (*ECUL)(this))
    this.ServiceCreate("ECON", (*ECON)(this))

    this.ServiceCreate("ECOL", (*ECOL)(this))
    this.ServiceCreate("ECLI", (*ECLI)(this))
    this.ServiceCreate("LST1", (*LST1)(this))
    this.ServiceCreate("LST2", (*LST2)(this))
    this.ServiceCreate("ERLS", (*ERLS)(this))
    this.ServiceCreate("ERLR", (*ERLR)(this))

    this.ServiceCreate("SND1", (*SND1)(this))
    this.ServiceCreate("SND2", (*SND2)(this))
    this.ServiceCreate("REDI", (*REDI)(this))
    this.ServiceCreate2("GTCX", new(GTCX))
    this.ServiceCreate("CNFG", (*CNFG)(this))
    this.ServiceCreate("CSV1", (*CSV1)(this))
    this.ServiceCreate("SLEP", (*SLEP)(this))
    this.ServiceCreate("USLP", (*USLP)(this))
    this.ServiceCreate("INIT", (*INIT)(this))
    this.ServiceCreate("MSQT", (*MSQT)(this))
    this.ServiceCreate("GTTO", (*GTTO)(this))
    this.ServiceCreate("PRNT", (*PRNT)(this))
    this.ServiceCreate("TRNS", (*TRNS)(this))
    this.ServiceCreate("TRN2", (*TRN2)(this))
    this.ServiceCreate("RDUL", (*RDUL)(this))
  }
}

type RDUL Server
  func (this *RDUL) Call() {
    this.ReadY()
    this.ReadS()
    this.ReadI()
    this.ReadW()
    this.ReadU()
  }

type TRN2 Server
  func (this *TRN2) Call() {
    this.ReadT_START(nil)
    this.i = this.ReadI()
    this.ReadT_END()
    this.j = this.ReadI()
  }

type TRNS Server
  func (this *TRNS) Call() {
    this.SendSTART ()
    this.SendT_START ("TRN2")
    this.SendI (9876)
    this.SendT_END ()
    this.SendI ( this.ReadI() )
    this.SendEND_AND_WAIT2 ("ECOI")
    this.ProcessEvent (WAIT_ONCE)
    this.SendSTART ()
    this.SendI (this.i)
    this.SendI (this.j)
    this.SendRETURN ()
  }

type PRNT Server
  func (this *PRNT) Call() {
    this.SendSTART()
    this.SendC(fmt.Sprintf("%d - %s", this.LinkGetCtxId(), this.ReadC()))
    this.SendEND_AND_WAIT2("WRIT")
    this.SendRETURN()
  }

type GTTO Server
  func (this *GTTO) Call() {
    this.SendSTART()
    this.SendC(this.ServiceGetToken())
    this.SendRETURN()
  }

type ECOC Server
  func (this *ECOC) Call() {
    this.SendSTART()
    this.SendC(this.ReadC())
    this.SendRETURN()
  }

type MSQT Server
  func (this *MSQT) Call() {
    this.SendSTART()
    if (this.ConfigGetDebug() != 0) {
      this.SendC ("debug")
      this.SendI (this.ConfigGetDebug())
    }
    if (this.ConfigGetIsString() == false) {
      this.SendC ("binary")
    }
    if (this.ConfigGetIsSilent() == true) {
      this.SendC ("silent")
    }
    this.SendC ("sOc")
    if this.ConfigGetIsServer() {
      this.SendC ("SERVER")
    } else {
      this.SendC ("CLIENT")
    }
    this.SendC ("pOc")
    if this.LinkIsParent() {
      this.SendC ("PARENT")
    } else {
      this.SendC ("CHILD")
    }
    this.SendRETURN()
  }

type INIT Server
  func (this *INIT) Call() {
    max := int(this.ReadGetNumItems())
    list := make([]string,max)
    this.SendSTART()
    for i:=0; i<max; i++ {
      list[i] = this.ReadC()
    }
    Init(list)
    this.SendRETURN()
  }

type USLP Server
  func (this *USLP) Call() {
    this.SendSTART()
    i := this.ReadI()
    time.Sleep(int64(i) * 1000)
    this.SendI(i)
    this.SendRETURN()
  }

type SLEP Server
  func (this *SLEP) Call() {
    this.SendSTART()
    i := this.ReadI()
    time.Sleep(int64(i) * 1000000000)
    this.SendI(i)
    this.SendRETURN()
  }

type CSV1 Server
  func (this *CSV1) Call() {
    var num int32

    // call an other service
    this.SendSTART()
    num = this.ReadI()
    num++
    this.SendI(num)
    this.SendEND_AND_WAIT ("CSV2", 10)

    // read the answer and send the result back
    this.SendSTART()
    num = this.ReadI()
    num++
    this.SendI(num)
    this.SendRETURN()
  }

type CNFG Server
  func (this *CNFG) Call() {
    this.SendSTART()
    this.SendO(this.ConfigGetIsServer())
    this.SendO(this.LinkIsParent())
    this.SendO(this.SlaveIs())
    this.SendO(this.ConfigGetIsString())
    this.SendO(this.ConfigGetIsSilent())
    this.SendO(this.LinkIsConnected())
    this.SendC(this.ConfigGetName())
    this.SendI(this.ConfigGetDebug())
    this.SendI(this.LinkGetCtxId())
    this.SendC(this.ServiceGetToken())
    this.SendRETURN()
  }

// test for callback without relationship to "*Server"
type GTCX int
  func (this *GTCX) Call(ctx *MqS) {
    ctx.SendSTART()
    ctx.SendI(ctx.LinkGetCtxId())
    ctx.SendRETURN()
  }

type REDI Server
  func (this *REDI) Call() {
    this.ReadI()
  }

type SND1 Server
  func (this *SND1) Call() {
    s := this.ReadC()
    id := this.ReadI()
    this.SendSTART()
    switch s {
      case "START": {
	parent := this.LinkGetParent()
	if (parent != nil && parent.GetSelf().(*Server).cl[id].LinkIsConnected()) {
	  this.cl[id].LinkCreateChild(parent.GetSelf().(*Server).cl[id].MqS)
	} else {
	  this.cl[id].LinkCreate(this.ConfigGetDebug())
	}
      }
      case "START2": {
	// object already created ERROR
	this.cl[id].LinkCreate(this.ConfigGetDebug())
	this.cl[id].LinkCreate(this.ConfigGetDebug())
      }
      case "START3": {
	parent := NewClient()
	// parent not connected ERROR
	this.cl[id].LinkCreateChild(parent.MqS)
      }
      case "START4": {
	// master not connected ERROR
	this.cl[id].SlaveWorker(0)
      }
      case "START5": {
	// the 'master' have to be a 'parent' without 'child' objects
	// 'slave' identifer out of range (0 <= 10000000 <= 1023)
	this.SlaveWorker(id, "--name", fmt.Sprintf("wk-cl-%d",id),
	  "--srvname", fmt.Sprintf("wk-sv-%d", id), "--thread")
      }
      case "STOP": {
	this.cl[id].LinkDelete()
      }
      case "SEND": {
	this.cl[id].SendSTART()
	TOK := this.ReadC()
	this.ReadProxy(this.cl[id].MqS)
	this.cl[id].SendEND(TOK)
      }
      case "WAIT": {
	this.cl[id].SendSTART()
	this.ReadProxy(this.cl[id].MqS)
	this.cl[id].SendEND_AND_WAIT("ECOI", 5)
	this.SendI(this.cl[id].ReadI()+1)
      }
      case "CALLBACK": {
	this.cl[id].SendSTART()
	this.ReadProxy(this.cl[id].MqS)
	this.cl[id].i = -1
	this.cl[id].SendEND_AND_CALLBACK2("ECOI", (*ECOI_CB)(this.cl[id]))
	this.cl[id].ProcessEvent2(10, WAIT_ONCE)
	this.SendI(this.cl[id].i+1)
      }
      case "ERR-1": {
	defer func() {
	  if x := recover(); x != nil {
	    this.ErrorSet(x)
	    this.SendI(this.ErrorGetNum())
	    this.SendC(this.ErrorGetText())
	    this.ErrorReset()
	    this.SendRETURN()
	  }
	}()
	this.cl[id].SendSTART()
	this.ReadProxy(this.cl[id].MqS)
	this.cl[id].SendEND_AND_WAIT("ECOI", 5)
      }
    }
    this.SendRETURN()
  }

type SetMyInt Server
  func (this *SetMyInt) Call() {
    this.SlaveGetMaster().GetSelf().(*Server).i = this.ReadI()
  }

type SND2 Server
  func (this *SND2) Call() {
    s  := this.ReadC()
    id := this.ReadI()
    cl := this.SlaveGet(id)

    this.SendSTART()
    switch s {
      case "CREATE": {
	var LIST  []string
	for this.ReadItemExists() {
	  LIST = append(LIST, this.ReadC())
	}
	LIST = append(LIST, "--name", "wk-cl-" + fmt.Sprintf("%d", id),
			"@", "--name", "wk-sv-" + fmt.Sprintf("%d", id))
	this.SlaveWorker(id, LIST...)
      }
      case "CREATE2": {
	c := NewClient()
	c.LinkCreate(this.ConfigGetDebug())
	this.SlaveCreate(id, c.MqS)
      }
      case "CREATE3": {
	c := NewClientERR()
	c.LinkCreate(this.ConfigGetDebug())
	this.SlaveCreate(id, c.MqS)
      }
      case "DELETE": {
	this.SlaveDelete(id)
	if this.SlaveGet(id) == nil {
	  this.SendC("OK")
	} else {
	  this.SendC("ERROR")
	}
      }
      case "SEND": {
	cl.SendSTART()
	TOK := this.ReadC()
	this.ReadProxy(cl)
	cl.SendEND(TOK)
      }
      case "WAIT": {
	cl.SendSTART()
	cl.SendN(this.ReadN())
	cl.SendEND_AND_WAIT("ECOI", 5)
	this.SendI(cl.ReadI()+1)
      }
      case "CALLBACK": {
	cl.SendSTART()
	this.ReadProxy(cl)
	this.i = -1
	cl.SendEND_AND_CALLBACK("ECOI", (*SetMyInt)(cl.GetSelf().(*Server)))
	cl.ProcessEvent2(10, WAIT_ONCE)
	this.SendI(this.i+1)
      }
      case "MqSendEND_AND_WAIT": {
	TOK := this.ReadC()
	cl.SendSTART()
	for this.ReadItemExists() {
	  this.ReadProxy(cl)
	}
	cl.SendEND_AND_WAIT(TOK, 5)
	for cl.ReadItemExists() {
	  cl.ReadProxy(this.MqS)
	}
      }
      case "MqSendEND": {
	TOK := this.ReadC()
	cl.SendSTART()
	for this.ReadItemExists() {
	  this.ReadProxy(cl)
	}
	cl.SendEND(TOK)
	return
      }
      case "ERR-1": {
	c := NewClientERR2()
	c.LinkCreate(this.ConfigGetDebug())
      }
      case "isSlave": {
	this.SendO(cl.SlaveIs())
      }
    }
    this.SendRETURN()
  }

type ECON Server
  func (this *ECON) Call() {
    this.SendSTART()
    this.SendC(this.ReadC() + "-" + this.ConfigGetName())
    this.SendRETURN()
  }

type ERLR Server
  func (this *ERLR) Call() {
    this.SendSTART()
    this.ReadL_START(nil)
    this.ReadL_START(nil)
    this.SendRETURN()
  }

type ERLS Server
  func (this *ERLS) Call() {
    this.SendSTART()
    this.SendL_START()
    this.SendU(this.ReadU())
    this.SendL_START()
    this.SendU(this.ReadU())
    this.SendRETURN()
  }

type LST1 Server
  func (this *LST1) Call() {
    this.SendSTART()
    this.SendL_END()
    this.SendRETURN()
  }

type LST2 Server
  func (this *LST2) Call() {
    this.SendSTART()
    this.ReadL_END()
    this.SendRETURN()
  }

func (this *Server) EchoList(doincr bool) {
  for this.ReadItemExists() {
    buf := this.ReadU()
    if (buf.GetType() == "L") {
      this.ReadL_START(buf)
      this.SendL_START()
      this.EchoList(doincr)
      this.SendL_END()
      this.ReadL_END()
    } else if (doincr) {
      this.SendI (buf.GetI()+1)
    } else {
      this.SendU(buf)
    }
  }
}

type ECOL Server
  func (this *ECOL) Call() {
    this.SendSTART()
    this.ReadL_START(nil)
    this.SendL_START()
    (*Server)(this).EchoList(false)
    this.SendL_END()
    this.ReadL_END()
    this.SendRETURN()
  }

type ECLI Server
  func (this *ECLI) Call() {
    opt := this.ReadU()
    doincr := (opt.GetType() == "C" && opt.GetC() == "--incr")
    if (!doincr) { this.ReadUndo() }
    this.SendSTART()
    (*Server)(this).EchoList(doincr)
    this.SendRETURN()
  }

type ERRT Server
  func (this *ERRT) Call() {
    this.SendSTART()
    this.ErrorC("MYERR", 9, this.ReadC())
    this.SendERROR()
  }

type Error Server
  func (this *Error) Call() {
    this.SendSTART()
    switch this.ServiceGetToken() {
      case "ERR2" : {
	this.SendC("some data")
	this.ErrorC("Ot_ERR2", 10, "some error")
      }
      case "ERR3" : this.SendRETURN()
      case "ERR4" : os.Exit(1)
    }
    this.SendRETURN()
  }

type CFG1 Server
  func (this *CFG1) Call() {
    this.SendSTART()
    switch this.ReadC() {
      case "Buffersize": {
	old := this.ConfigGetBuffersize()
	this.ConfigSetBuffersize (this.ReadI())
	this.SendI (this.ConfigGetBuffersize())
	this.ConfigSetBuffersize (old)
      }
      case "Debug": {
	old := this.ConfigGetDebug()
	this.ConfigSetDebug (this.ReadI())
	this.SendI (this.ConfigGetDebug())
	this.ConfigSetDebug (old)
      }
      case "Timeout": {
	old := this.ConfigGetTimeout()
	this.ConfigSetTimeout (this.ReadW())
	this.SendW (this.ConfigGetTimeout())
	this.ConfigSetTimeout (old)
      }
      case "Name": {
	old := this.ConfigGetName()
	this.ConfigSetName (this.ReadC())
	this.SendC (this.ConfigGetName())
	this.ConfigSetName (old)
      }
      case "SrvName": {
	old := this.ConfigGetSrvName()
	this.ConfigSetSrvName (this.ReadC())
	this.SendC (this.ConfigGetSrvName())
	this.ConfigSetSrvName (old)
      }
      case "Ident": {
	old := this.ConfigGetIdent()
	this.ConfigSetIdent (this.ReadC())
	check := this.LinkGetTargetIdent() == this.ReadC()
	this.SendSTART()
	this.SendC (this.ConfigGetIdent())
	this.SendO (check)
	this.ConfigSetIdent (old)
      }
      case "IsSilent": {
	old := this.ConfigGetIsSilent()
	this.ConfigSetIsSilent (this.ReadO())
	this.SendO (this.ConfigGetIsSilent())
	this.ConfigSetIsSilent (old)
      }
      case "IsString": {
	old := this.ConfigGetIsString()
	this.ConfigSetIsString (this.ReadO())
	this.SendO (this.ConfigGetIsString())
	this.ConfigSetIsString (old)
      }
      case "IoUds": {
	old := this.ConfigGetIoUdsFile()
	this.ConfigSetIoUdsFile (this.ReadC())
	this.SendC (this.ConfigGetIoUdsFile())
	this.ConfigSetIoUdsFile (old)
      }
      case "IoTcp": {
	h  := this.ConfigGetIoTcpHost   ()
	p  := this.ConfigGetIoTcpPort   ()
	mh := this.ConfigGetIoTcpMyHost ()
	mp := this.ConfigGetIoTcpMyPort ()
	hv := this.ReadC()
	pv := this.ReadC()
	mhv := this.ReadC()
	mpv := this.ReadC()
	this.ConfigSetIoTcp (hv,pv,mhv,mpv)
	this.SendC (this.ConfigGetIoTcpHost())
	this.SendC (this.ConfigGetIoTcpPort())
	this.SendC (this.ConfigGetIoTcpMyHost())
	this.SendC (this.ConfigGetIoTcpMyPort())
	this.ConfigSetIoTcp (h,p,mh,mp)
      }
      case "IoPipe": {
	old := this.ConfigGetIoPipeSocket()
	this.ConfigSetIoPipeSocket (this.ReadI())
	this.SendI (this.ConfigGetIoPipeSocket())
	this.ConfigSetIoPipeSocket (old)
      }
      case "StartAs": {
	old := this.ConfigGetStartAs()
	this.ConfigSetStartAs (START(this.ReadI()))
	this.SendI (int32(this.ConfigGetStartAs()))
	this.ConfigSetStartAs (old)
      }
      default: {
	this.ErrorC ("CFG1", 1, "invalid command: ")
      }
    }
    this.SendRETURN()
  }

type BUF3 Server
  func (this *BUF3) Call() {
    this.SendSTART()
    buf := this.ReadU()
    this.SendC(buf.GetType())
    this.SendU(buf)
    this.SendI(this.ReadI())
    this.SendU(buf)
    this.SendRETURN()
  }

type BUF2 Server
  func (this *BUF2) Call() {
    this.SendSTART()
    for i:=0; i<3; i++ {
      buf := this.ReadU()
      this.SendC(buf.GetType())
      this.SendU(buf)
    }
    this.SendRETURN()
  }

type BUF1 Server
  func (this *BUF1) Call() {
    buf := this.ReadU()
    typ := buf.GetType()
    this.SendSTART()
    this.SendC(typ)
    switch (typ) {
      case "Y": this.SendY(buf.GetY())
      case "O": this.SendO(buf.GetO())
      case "S": this.SendS(buf.GetS())
      case "I": this.SendI(buf.GetI())
      case "F": this.SendF(buf.GetF())
      case "W": this.SendW(buf.GetW())
      case "D": this.SendD(buf.GetD())
      case "C": this.SendC(buf.GetC())
      case "B": this.SendB(buf.GetB())
    }
    this.SendRETURN()
  }

type ECOB Server
  func (this *ECOB) Call() {
    this.SendSTART()
    this.SendB(this.ReadB())
    this.SendRETURN()
  }

type ECOD Server
  func (this *ECOD) Call() {
    this.SendSTART()
    this.SendD(this.ReadD())
    this.SendRETURN()
  }

type ECOF Server
  func (this *ECOF) Call() {
    this.SendSTART()
    this.SendF(this.ReadF())
    this.SendRETURN()
  }

type SETU Server
  func (this *SETU) Call() {
    this.buf = this.ReadU()
  }

type GETU Server
  func (this *GETU) Call() {
    this.SendSTART()
    this.SendU(this.buf)
    this.SendRETURN()
  }

type ECOO Server
  func (this *ECOO) Call() {
    this.SendSTART()
    this.SendO(this.ReadO())
    this.SendRETURN()
  }

type ECOY Server
  func (this *ECOY) Call() {
    this.SendSTART()
    this.SendY(this.ReadY())
    this.SendRETURN()
  }

type ECOS Server
  func (this *ECOS) Call() {
    this.SendSTART()
    this.SendS(this.ReadS())
    this.SendRETURN()
  }

type ECOI Server
  func (this *ECOI) Call() {
    this.SendSTART()
    this.SendI(this.ReadI())
    this.SendRETURN()
  }

type ECOW Server
  func (this *ECOW) Call() {
    this.SendSTART()
    this.SendW(this.ReadW())
    this.SendRETURN()
  }

type ECOU Server
  func (this *ECOU) Call() {
    this.SendSTART()
    this.SendU(this.ReadU())
    this.SendRETURN()
  }

type ECUL Server
  func (this *ECUL) Call() {
    this.SendSTART()
    this.SendY(this.ReadY())
    this.SendS(this.ReadS())
    this.SendI(this.ReadI())
    this.SendW(this.ReadW())
    this.ReadProxy(this.MqS)
    this.SendRETURN()
    //this.ErrorC("ECUL",-1,"fehler")
  }

func main() {
  srv := NewServer()
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
  srv.ProcessEvent(WAIT_FOREVER)
}

