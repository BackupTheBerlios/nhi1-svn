#+
#§  \file       theLink/example/ruby/Filter1.rb
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

class Filter1(MqS):
  def __init__(self):
    self.ConfigSetFactory(lambda: Filter1())
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
  srv.ProcessEvent(wait="FOREVER")
except:
  srv.ErrorSet()
finally:
  srv.Exit()


