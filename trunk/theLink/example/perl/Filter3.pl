#+
#§  \file       theLink/example/perl/Filter3.pl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

use strict;
use Net::PerlMsgque;

$| = 1;

package Filter3;
use base qw(Net::PerlMsgque::MqS);

  sub Filter {
    my $ctx = shift;
    my $ftr = $ctx->ConfigGetFilter();
    $ftr->SendSTART();
    $ftr->SendBDY($ctx->ReadBDY());
    if ($ctx->ConfigGetIsTransaction()) {
      $ftr->SendEND_AND_WAIT($ctx->ConfigGetToken());
      $ctx->SendSTART();
      $ctx->SendBDY($ftr->ReadBDY());
    } else {
      $ftr->SendEND($ctx->ConfigGetToken());
    }
    $ctx->SendRETURN();
  }

  sub ServerSetup {
    my $ctx = shift;
    my $ftr = $ctx->ConfigGetFilter();
    $ctx->ServiceCreate("+ALL", \&Filter);
    $ftr->ServiceCreate("+ALL", \&Filter);
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetName("filter");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetFactory(sub {new Filter3()});
    return $ctx;
  }


package main;

  our $srv = new Filter3();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

1;

