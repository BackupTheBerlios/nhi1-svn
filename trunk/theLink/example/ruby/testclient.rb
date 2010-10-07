#+
#§  \file       theLink/example/ruby/testclient.rb
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
import sys
import os
## setup the clients
server = os.path.join(os.path.dirname(sys.argv[0]), "testserver.py")
## create object
c0 = MqS()
c00 = MqS()
c01 = MqS()
c000 = MqS()
c1 = MqS()
c10 = MqS()
c100 = MqS()
c101 = MqS()
## setup object link
c0.LinkCreate(["c0",    "--srvname", "s0",  "--debug", os.environ.get("TS_DEBUG"), "@", "python", server])
c00.LinkCreateChild(c0,   ["c00",   "--name", "c00", "--srvname", "s00"])
c01.LinkCreateChild(c0,	  ["c01",   "--name", "c01", "--srvname", "s01"])
c000.LinkCreateChild(c00, ["c000",  "--name", "c000", "--srvname", "s000"])
c1.LinkCreate(["c1",    "--srvname", "s1"] + sys.argv[1:])
c10.LinkCreateChild(c1,   ["c10",   "--name", "c10", "--srvname", "s10"])
c100.LinkCreateChild(c10, ["c100",  "--name", "c100", "--srvname", "s100"])
c101.LinkCreateChild(c10, ["c101",  "--name", "c101", "--srvname", "s101"])
## my helper
def Get(ctx):
  """ get data from pymsgque """
  RET = ''
  ctx.SendSTART()
  ctx.SendEND_AND_WAIT("GTCX")
  RET += ctx.ConfigGetName()
  RET += "+"
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  RET += ctx.ReadC()
  return RET
## do the tests
print(Get(c0))
print(Get(c00))
print(Get(c01))
print(Get(c000))
print(Get(c1))
print(Get(c10))
print(Get(c100))
print(Get(c101))
## do the cleanup
c0.Delete()
c1.Delete()
