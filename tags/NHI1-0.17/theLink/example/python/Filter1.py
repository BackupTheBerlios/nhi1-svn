#+
#:  \file       theLink/example/python/Filter1.py
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

import sys
from pymsgque import *

class Filter1(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetName("filter")
    self.ConfigSetServerSetup(self.ServerSetup)
    self.data = []
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceCreate("+EOF", self.EOFcmd)
  def FTRcmd(ctx):
    L = []
    while ctx.ReadItemExists():
      L.append("<" + ctx.ReadC() + ">")
    ctx.data.append(L)
    ctx.SendRETURN()
  def EOFcmd(ctx):
    ftr = ctx.ServiceGetFilter()
    for L in ctx.data:
      ftr.SendSTART()
      for I in L:
        ftr.SendC(I)
      ftr.SendEND_AND_WAIT("+FTR")
    ftr.SendSTART()
    ftr.SendEND_AND_WAIT("+EOF")
    ctx.SendRETURN()
srv = Filter1()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()

