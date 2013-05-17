#+
#:  \file       theLink/example/python/Filter5.py
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

import sys
from pymsgque import *

# F1 ******************************************************

class F1(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceProxy("+EOF")
  def FTRcmd(ctx):
    ftr = ctx.ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F1")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ctx.ConfigGetStatusIs())
    while ctx.ReadItemExists():
      ftr.SendC(ctx.ReadC())
    ftr.SendEND_AND_WAIT("+FTR")
    ctx.SendRETURN()

# F2 ******************************************************

class F2(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceProxy("+EOF")
  def FTRcmd(ctx):
    ftr = ctx.ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F2")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ctx.ConfigGetStatusIs())
    while ctx.ReadItemExists():
      ftr.SendC(ctx.ReadC())
    ftr.SendEND_AND_WAIT("+FTR")
    ctx.SendRETURN()

# F3 ******************************************************

class F3(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceProxy("+EOF")
  def FTRcmd(ctx):
    ftr = ctx.ServiceGetFilter()
    ftr.SendSTART()
    ftr.SendC("F3")
    ftr.SendC(ftr.ConfigGetName())
    ftr.SendI(ftr.ConfigGetStartAs())
    ftr.SendI(ctx.ConfigGetStatusIs())
    while ctx.ReadItemExists():
      ftr.SendC(ctx.ReadC())
    ftr.SendEND_AND_WAIT("+FTR")
    ctx.SendRETURN()

# main ****************************************************

FactoryAdd(F1)
FactoryAdd(F2)
FactoryAdd(F3)

srv = FactoryGetCalled(sys.argv[1]).New()
try:
  srv.LinkCreate(sys.argv[1:])
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()

