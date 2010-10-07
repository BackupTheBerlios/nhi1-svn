#+
#§  \file       theLink/example/ruby/Filter2.rb
#§  \brief      \$Id: LbMain 26 2009-12-03 11:48:43Z aotto1968 $
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 26 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

require "rubymsgque"

class Filter2(MqS):
  def __init__(self):
    self.ConfigSetFactory(lambda: Filter2())
    self.ConfigSetName("filter")
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(self):
    self.ServiceCreate("+FTR", self.FTRcmd)
    self.ServiceProxy ("+EOF")
  def FTRcmd(ctx):
    raise Exception("my error")
srv = Filter2()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()

