#+
#:  \file       theLink/example/perl/MyServer.pl
#:  \brief      \$Id$
#:  
#:  (C) 2009 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

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
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    bless $ctx, $class;
    return $ctx;
  }

package main;

  our $srv = MqFactoryS::Add("MyServer")->New();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

