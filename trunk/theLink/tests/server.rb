require "rubymsgque"

class Server < MqS

  def initialize
    #puts("initialize-1 -----------" + self.to_s)
    self.ConfigSetName("server")
    self.ConfigSetIdent("test-server")
    self.ConfigSetServerSetup(self.method(:ServerSetup))
    self.ConfigSetServerCleanup(self.method(:ServerCleanup))
    self.ConfigSetFactory(lambda {Server.new})

    super()
  end

  def ServerCleanup
    #puts("ServerCleanup -----------" + self.to_s)
  end

  def ServerSetup
    #puts("ServerSetup ----------- " + self.to_s)

    # add "master" services here
    self.ServiceCreate("ECOU", self.method(:ECOU))
  end

  def ECOU
    self.SendSTART
    self.SendU(self.ReadU())
    self.SendRETURN
  end

end

## --------------------------------------------------------
##    Main
##

#print("1111")
#sys.stdout.flush()

srv = Server.new

begin
  srv.LinkCreate(ARGV)
  srv.LogC("test",1,"this is the log test\n")
  srv.ProcessEvent(MqS::Timeout::DEFAULT, MqS::Wait::FOREVER)
rescue exception => ex
  srv.ErrorSet(ex)
ensure
  srv.Exit()
end

# vim: softtabstop=2:tabstop=8:shiftwidth=2:expandtab
