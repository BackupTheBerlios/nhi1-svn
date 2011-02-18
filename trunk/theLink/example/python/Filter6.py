#+
#§  \file       theLink/example/python/Filter6.py
#§  \brief      \$Id$
#§  
#§  (C) 2011 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

import sys
import os
from pymsgque import *

class Filter6(MqS):
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
    ctx.ServiceStorage("PRNT")
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
    if ctx.StorageCount() == 0:
      ctx.ErrorSetCONTINUE()
    else:
      id = 0
      try:
        ftr = ctx.ServiceGetFilter()
        ftr.LinkConnect()
        id = ctx.StorageSelect()
        ctx.ReadForward(ftr)
      except:
        ctx.ErrorSet()
        if ctx.ErrorIsEXIT():
          ctx.ErrorReset()
          return
        else:
          ctx.ErrorWrite()
      ctx.StorageDelete(id)
  def FilterIn(ctx):
    ctx.StorageInsert()
    ctx.SendRETURN()

srv = FactoryAdd("transFilter", Filter6).New();
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()
