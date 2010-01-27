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
    ctx.ConfigSetEvent(ctx.Event)
    ctx.itms = []
    ctx.file = None
    MqS.__init__(ctx)
  def ServerSetup(ctx):
    ctx.ServiceCreate("LOGF", ctx.LOGF)
    ctx.ServiceCreate("EXIT", ctx.EXIT)
    ctx.ServiceCreate("+ALL", ctx.FilterIn)
  def ErrorWrite(ctx):
    if ctx.file is None:
      ctx.ErrorPrint()
    else:
      FH = open(ctx.file, "a")
      FH.write("ERROR: " + ctx.ErrorGetText() + "\n")
      FH.close
  def LOGF(ctx):
    ftr = ctx.ServiceGetFilter()
    ftr.file = ctx.ReadC()
    if (ftr.LinkGetTargetIdent() == "transFilter"):
      ftr.SendSTART()
      ftr.SendC(ftr.file)
      ftr.SendEND_AND_WAIT("LOGF")
    ctx.SendRETURN()
  def EXIT(ctx):
    sys.exit();
  def Event(ctx):
    if (len(ctx.itms) == 0):
      ctx.ErrorSetCONTINUE()
    else:
      (token, isTransaction, bdy) = ctx.itms[0]
      sys.stdout.flush()
      ftr = ctx.ServiceGetFilter()
      try:
        ftr.LinkConnect()
        ftr.SendSTART()
        ftr.SendBDY(bdy)
        if (isTransaction):
          ftr.SendEND_AND_WAIT(token)
        else:
          ftr.SendEND(token)
      except:
        if ftr.ErrorIsEXIT():
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

