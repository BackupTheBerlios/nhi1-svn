
use strict;
use Net::PerlMsgque;

# F1 ************************************************

package F1;
use base qw(Net::PerlMsgque::MqS);

sub FTRcmd {
  my $ctx = shift;
  my $ftr = $ctx->ServiceGetFilter();
  $ftr->SendSTART();
  $ftr->SendC("F1");
  $ftr->SendC($ftr->ConfigGetName());
  $ftr->SendC($ftr->ConfigGetStartAs());
  $ftr->SendC($ctx->ConfigGetStatusIs());
  while ($ctx->ReadItemExists()) {
    $ftr->SendC($ctx->ReadC());
  }
  $ftr->SendEND_AND_WAIT("+FTR");
  $ctx->SendRETURN();
}

sub ServerSetup {
  my $ctx = shift;
  $ctx->ServiceCreate("+FTR", \&FTRcmd);
  $ctx->ServiceProxy("+EOF");
}

sub new {
  my $class = shift;
  my $ctx = $class->SUPER::new(@_);
  $ctx->ConfigSetServerSetup(\&ServerSetup);
  return $ctx;
}

# F2 ************************************************

package F2;
use base qw(Net::PerlMsgque::MqS);

sub FTRcmd {
  my $ctx = shift;
  my $ftr = $ctx->ServiceGetFilter();
  $ftr->SendSTART();
  $ftr->SendC("F2");
  $ftr->SendC($ftr->ConfigGetName());
  $ftr->SendC($ftr->ConfigGetStartAs());
  $ftr->SendC($ctx->ConfigGetStatusIs());
  while ($ctx->ReadItemExists()) {
    $ftr->SendC($ctx->ReadC());
  }
  $ftr->SendEND_AND_WAIT("+FTR");
  $ctx->SendRETURN();
}

sub ServerSetup {
  my $ctx = shift;
  $ctx->ServiceCreate("+FTR", \&FTRcmd);
  $ctx->ServiceProxy("+EOF");
}

sub new {
  my $class = shift;
  my $ctx = $class->SUPER::new(@_);
  $ctx->ConfigSetServerSetup(\&ServerSetup);
  return $ctx;
}

# F3 ************************************************

package F3;
use base qw(Net::PerlMsgque::MqS);

sub FTRcmd {
  my $ctx = shift;
  my $ftr = $ctx->ServiceGetFilter();
  $ftr->SendSTART();
  $ftr->SendC("F3");
  $ftr->SendC($ftr->ConfigGetName());
  $ftr->SendC($ftr->ConfigGetStartAs());
  $ftr->SendC($ctx->ConfigGetStatusIs());
  while ($ctx->ReadItemExists()) {
    $ftr->SendC($ctx->ReadC());
  }
  $ftr->SendEND_AND_WAIT("+FTR");
  $ctx->SendRETURN();
}

sub ServerSetup {
  my $ctx = shift;
  $ctx->ServiceCreate("+FTR", \&FTRcmd);
  $ctx->ServiceProxy("+EOF");
}

sub new {
  my $class = shift;
  my $ctx = $class->SUPER::new(@_);
  $ctx->ConfigSetServerSetup(\&ServerSetup);
  return $ctx;
}

# main **********************************************

package main;

  Net::PerlMsgque::FactoryAdd("F1");
  Net::PerlMsgque::FactoryAdd("F2");
  Net::PerlMsgque::FactoryAdd("F3");

  our $srv = Net::PerlMsgque::FactoryCall($ARGV[0]);
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

