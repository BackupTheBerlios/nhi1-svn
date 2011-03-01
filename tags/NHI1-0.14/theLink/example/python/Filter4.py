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
  def __init__(ctx, tmpl=None):
    ctx.ConfigSetIgnoreExit(True)
    ctx.ConfigSetServerSetup(ctx.ServerSetup)
    ctx.ConfigSetServerCleanup(ctx.ServerCleanup)
    ctx.ConfigSetEvent(ctx.Event)
    ctx.itms = []
    ctx.FH = None
    MqS.__init__(ctx)
  def ServerCleanup(ctx):
    if ctx.FH != None:
      ctx.FH.close()
  def ServerSetup(ctx):
    ftr = ctx.ServiceGetFilter()
    ctx.ServiceCreate("LOGF", ctx.LOGF)
    ctx.ServiceCreate("EXIT", ctx.EXIT)
    ctx.ServiceCreate("+ALL", ctx.FilterIn)
    ftr.ServiceCreate("WRIT", ftr.WRIT)
    ftr.ServiceProxy("+TRT")
  def ErrorWrite(ctx):
    ctx.FH.write("ERROR: " + ctx.ErrorGetText() + "\n")
    ctx.FH.flush()
    ctx.ErrorReset()
  def LOGF(ctx):
    ftr = ctx.ServiceGetFilter()
    if ftr.LinkGetTargetIdent() == "transFilter":
      ctx.ReadForward(ftr)
    else:
      ctx.FH = open(ctx.ReadC(), "a")
    ctx.SendRETURN()
  def WRIT(ftr):
    FH = ftr.ServiceGetFilter().FH
    FH.write(ftr.ReadC() + "\n")
    FH.flush()
    ftr.SendRETURN()
  def EXIT(ctx):
    os._exit(1)
  def Event(ctx):
    if len(ctx.itms) == 0:
      ctx.ErrorSetCONTINUE()
    else:
      try:
        ftr = ctx.ServiceGetFilter()
        ftr.LinkConnect()
        ctx.ReadForward(ftr,ctx.itms[0])
      except:
        ctx.ErrorSet()
        if ctx.ErrorIsEXIT():
          ctx.ErrorReset()
          return
        else:
          ctx.ErrorWrite()
      ctx.itms.pop(0)
  def FilterIn(ctx):
    ctx.itms.append(ctx.ReadDUMP())
    ctx.SendRETURN()

FactoryDefault("transFilter", Filter4)
srv = Filter4()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()
