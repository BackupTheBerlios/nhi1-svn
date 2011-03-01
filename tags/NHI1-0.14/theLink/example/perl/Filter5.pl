#+
#§  \file       theLink/example/perl/Filter5.pl
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

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

  MqFactoryS::Add("F1");
  MqFactoryS::Add("F2");
  MqFactoryS::Add("F3");

  our $srv = MqFactoryS::GetCalled($ARGV[0])->New();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

