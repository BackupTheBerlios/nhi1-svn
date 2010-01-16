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
    ctx.ConfigSetFactory(lambda: Filter4())
    ctx.ConfigSetServerSetup(ctx.ServerSetup)
    ctx.ConfigSetEvent(ctx.Event)
    ctx.itms = []
    MqS.__init__(ctx)
  def ServerSetup(ctx):
    ctx.ServiceCreate("+ALL", ctx.FilterIn)
  def Event(ctx):
    if (len(ctx.itms) > 0):
      (token, isTransaction, bdy) = ctx.itms.pop(0)
      try:
        ftr = ctx.ServiceGetFilter()
        ftr.SendSTART()
        ftr.SendBDY(bdy)
        if (isTransaction):
          ftr.SendEND_AND_WAIT(token)
        else:
          ftr.SendEND(token)
      except:
        ctx.ErrorSet()
        ctx.ErrorPrint()
        ctx.ErrorReset()
    else:
      ctx.ErrorSetCONTINUE()
  def FilterIn(ctx):
    ctx.itms.append([ctx.ServiceGetToken(), ctx.ServiceIsTransaction(), ctx.ReadBDY()])
    ctx.SendRETURN()
srv = Filter4()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

