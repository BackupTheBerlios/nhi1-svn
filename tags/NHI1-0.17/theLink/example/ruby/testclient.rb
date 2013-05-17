#+
#:  \file       theLink/example/ruby/testclient.rb
#:  \brief      \$Id$
#:  
#:  (C) 2010 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:
require "rubymsgque"
## setup the clients
server = File.join(File.dirname($0), "testserver.rb")
## create object
c0 = MqS.new
c00 = MqS.new
c01 = MqS.new
c000 = MqS.new
c1 = MqS.new
c10 = MqS.new
c100 = MqS.new
c101 = MqS.new
## setup object link
c0.LinkCreate("c0",    "--srvname", "s0",  "--debug", ENV['TS_DEBUG'], "@", ENV['RUBY'], server)
c00.LinkCreateChild(c0,   "c00",   "--name", "c00", "--srvname", "s00")
c01.LinkCreateChild(c0,	  "c01",   "--name", "c01", "--srvname", "s01")
c000.LinkCreateChild(c00, "c000",  "--name", "c000", "--srvname", "s000")
c1.LinkCreate("c1",    "--srvname", "s1", ARGV)
c10.LinkCreateChild(c1,   "c10",   "--name", "c10", "--srvname", "s10")
c100.LinkCreateChild(c10, "c100",  "--name", "c100", "--srvname", "s100")
c101.LinkCreateChild(c10, "c101",  "--name", "c101", "--srvname", "s101")
## my helper
def Get(ctx)
  ret = ""
  ctx.SendSTART()
  ctx.SendEND_AND_WAIT("GTCX")
  ret += ctx.ConfigGetName()
  ret += "+"
  ret += ctx.ReadC()
  ret += ctx.ReadC()
  ret += ctx.ReadC()
  ret += ctx.ReadC()
  ret += ctx.ReadC()
  ret += ctx.ReadC()
  return ret
end
## do the tests
$stdout.puts(Get(c0))
$stdout.puts(Get(c00))
$stdout.puts(Get(c01))
$stdout.puts(Get(c000))
$stdout.puts(Get(c1))
$stdout.puts(Get(c10))
$stdout.puts(Get(c100))
$stdout.puts(Get(c101))
$stdout.flush
## do the cleanup
c0.Delete()
c1.Delete()
