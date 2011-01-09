#+
#§  \file       theLink/example/python/manfilter.py
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

import sys
from pymsgque import *

class manfilter(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetName("ManFilter")
    self.ConfigSetServerSetup(self.ServerSetup)
    self.data = []
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceProxy("+EOF")
  def FTRcmd(ctx):
    ftr = ctx.ServiceGetFilter()
    ftr.SendSTART()
    while ctx.ReadItemExists():
      ftr.SendC("<" + ctx.ReadC() + ">")
    ftr.SendEND_AND_WAIT("+FTR")
    ctx.SendRETURN()

srv = manfilter()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()
