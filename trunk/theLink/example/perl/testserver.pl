#+
#§  \file       example/perl/testserver.pl
#§  \brief      \$Id: testserver.pl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
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
    $ctx->SendI($ctx->ConfigGetCtxId());
    $ctx->SendC("+");
    if ($ctx->ConfigGetIsParent()) {
      $ctx->SendI(-1);
    } else {
      $ctx->SendI($ctx->ConfigGetParent()->ConfigGetCtxId());
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
    $ctx->ConfigSetName("testserver");
    $ctx->ConfigSetServerSetup(\&ServerConfig);
    $ctx->ConfigSetFactory(sub {new testserver()});
    return $ctx;
  }

package main;

  our $srv = new testserver();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



