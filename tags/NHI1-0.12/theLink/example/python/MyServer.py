#+
#§  \file       theLink/example/python/MyServer.py
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

class MyServer(MqS):
  def __init__(self, tmpl=None):
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def HLWO(self):
    self.SendSTART()
    self.SendC("Hello World")
    self.SendRETURN()
  def ServerSetup(self):
    self.ServiceCreate("HLWO",self.HLWO)

srv = FactoryNew("MyServer", MyServer);
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(MqS_WAIT_FOREVER)
except:
  srv.ErrorSet()
finally:
  srv.Exit()
