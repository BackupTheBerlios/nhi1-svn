#+
#§  \file       theLink/example/ruby/Filter3.rb
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

class Filter3(MqS):
  def __init__(self):
    self.ConfigSetFactory(lambda: Filter3())
    self.ConfigSetName("Filter3")
    self.ConfigSetServerSetup(self.ServerSetup)
    MqS.__init__(self)
  def ServerSetup(ctx):
    ftr =ctx.ServiceGetFilter()
    ctx.ServiceProxy("+ALL")
    ctx.ServiceProxy("+TRT")
    ftr.ServiceProxy("+ALL")
    ftr.ServiceProxy("+TRT")
srv = Filter3()
try:
  srv.LinkCreate(sys.argv)
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()




