#+
#§  \file       example/perl/MyServer.pl
#§  \brief      \$Id: MyServer.pl 507 2009-11-28 15:18:46Z dev1usr $
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

package MyServer;
use base qw(Net::PerlMsgque::MqS);

  sub HLWO {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendC("Hello World");
    $ctx->SendRETURN();
  }

  sub ServerSetup {
    my $ctx = shift;
    $ctx->ServiceCreate("HLWO",\&HLWO)
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetName("MyServer");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetFactory(sub {new MyServer()});
    return $ctx;
  }

package main;

  our $srv = new MyServer();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



