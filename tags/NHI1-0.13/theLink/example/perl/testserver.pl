#+
#§  \file       theLink/example/perl/testserver.pl
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

package testserver;
use base qw(Net::PerlMsgque::MqS);

  sub GTCX {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendI($ctx->LinkGetCtxId());
    $ctx->SendC("+");
    if ($ctx->LinkIsParent()) {
      $ctx->SendI(-1);
    } else {
      $ctx->SendI($ctx->LinkGetParent()->LinkGetCtxId());
    }
    $ctx->SendC("+");
    $ctx->SendC($ctx->ConfigGetName());
    $ctx->SendC(":");
    $ctx->SendRETURN();
  }

  sub ServerConfig {
    my $ctx = shift;
    $ctx->ServiceCreate("GTCX",\&GTCX)
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetServerSetup(\&ServerConfig);
    return $ctx;
  }

package main;

  our $srv = MqFactoryS::Add("testserver")->New();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();








