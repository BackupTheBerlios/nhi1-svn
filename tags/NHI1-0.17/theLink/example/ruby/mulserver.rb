#+
#:  \file       theLink/example/ruby/mulserver.rb
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

class MulServer < MqS
  def initialize()
    super()
    ConfigSetServerSetup(method(:ServerSetup))
  end
  def MMUL
    SendSTART()
    SendD(ReadD() * ReadD())
    SendRETURN()
  end
  def ServerSetup()
    ServiceCreate("MMUL",method(:MMUL))
  end
end

srv = FactoryAdd("mulserver", MulServer).New()
begin
  srv.LinkCreate($0,ARGV)
  srv.ProcessEvent(MqS::WAIT_FOREVER)
rescue Exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

