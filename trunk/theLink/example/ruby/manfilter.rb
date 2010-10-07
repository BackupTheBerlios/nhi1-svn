#+
#§  \file       theLink/example/ruby/manfilter.rb
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

class manfilter(MqS):
  def __init__(self):
    self.ConfigSetFactory(lambda: manfilter())
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
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()


