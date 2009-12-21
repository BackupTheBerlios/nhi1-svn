#+
#§  \file       theLink/example/python/Filter3.py
#§  \brief      \$Id: Filter3.py 25 2009-12-03 11:05:46Z aotto1968 $
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

class Filter3(MqS):
  def __init__(self):
    self.ConfigSetFactory(lambda: Filter3())
    self.ConfigSetName("filter")
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(ctx):
    ftr =ctx.ConfigGetFilter()
    ctx.ServiceCreate("+ALL", ctx.Filter)
    ftr.ServiceCreate("+ALL", ctx.Filter)
  def Filter(ctx):
    ftr = ctx.ConfigGetFilter()
    ftr.SendSTART()
    ftr.SendBDY(ctx.ReadBDY())
    if (ctx.ConfigGetIsTransaction()):
      ftr.SendEND_AND_WAIT(ctx.ConfigGetToken())
      ctx.SendSTART()
      ctx.SendBDY(ftr.ReadBDY())
    else:
      ftr.SendEND(ctx.ConfigGetToken())
    ctx.SendRETURN()
srv = Filter3()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

