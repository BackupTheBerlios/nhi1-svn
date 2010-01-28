#+
#§  \file       theLink/example/python/Filter4.py
#§  \brief      \$Id: Filter4.py 25 2009-12-04 11:05:46Z aotto1968 $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 25 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

import sys
from pymsgque import *

class Filter4(MqS):
  def __init__(ctx):
    ctx.ConfigSetIgnoreExit(True)
    ctx.ConfigSetIdent("transFilter")
    ctx.ConfigSetFactory(lambda: Filter4())
    ctx.ConfigSetServerSetup(ctx.ServerSetup)
    ctx.ConfigSetServerCleanup(ctx.ServerCleanup)
    ctx.ConfigSetEvent(ctx.Event)
    ctx.itms = []
    ctx.FH = None
    MqS.__init__(ctx)
  def ServerCleanup(ctx):
    ftr = ctx.ServiceGetFilter()
    if ftr.FH != None:
      close (ftr.FH)
  def ServerSetup(ctx):
    ftr = ctx.ServiceGetFilter()
    ctx.ServiceCreate("LOGF", ctx.LOGF)
    ctx.ServiceCreate("EXIT", ctx.EXIT)
    ctx.ServiceCreate("+ALL", ctx.FilterIn)
    ftr.ServiceCreate("WRIT", ftr.WRIT)
  def ErrorWrite(ftr):
    ftr.FH.write("ERROR: " + ftr.ErrorGetText() + "\n")
    ftr.FH.flush()
    ftr.ErrorReset()
  def LOGF(ctx):
    ftr = ctx.ServiceGetFilter()
    if ftr.LinkGetTargetIdent() == "transFilter":
      ftr.SendSTART()
      ftr.SendC(ctx.ReadC())
      ftr.SendEND_AND_WAIT("LOGF")
    else:
      ftr.FH = open(ctx.ReadC(), "a")
    ctx.SendRETURN()
  def WRIT(ftr):
    ftr.FH.write(ftr.ReadC() + "\n")
    ftr.FH.flush()
    ftr.SendRETURN()
  def EXIT(ctx):
    sys.exit();
  def Event(ctx):
    if len(ctx.itms) == 0:
      ctx.ErrorSetCONTINUE()
    else:
      (token, isTransaction, bdy) = ctx.itms[0]
      ftr = ctx.ServiceGetFilter()
      try:
        ftr.LinkConnect()
        ftr.SendSTART()
        ftr.SendBDY(bdy)
        if isTransaction:
          ftr.SendEND_AND_WAIT(token)
        else:
          ftr.SendEND(token)
      except:
        ftr.ErrorSet()
        if ftr.ErrorIsEXIT():
          ftr.ErrorReset()
          return
        else:
          ftr.ErrorWrite()
      ctx.itms.pop(0)
  def FilterIn(ctx):
    ctx.itms.append([ctx.ServiceGetToken(), ctx.ServiceIsTransaction(), ctx.ReadBDY()])
    ctx.SendRETURN()
try:
  srv = Filter4()
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

