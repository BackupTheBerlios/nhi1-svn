#+
#§  \file       theLink/example/perl/mulserver.pl
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

package MulServer;
use base qw(Net::PerlMsgque::MqS);

  sub MMUL {
    my $ctx = shift;
    $ctx->SendSTART();
    $ctx->SendD($ctx->ReadD() * $ctx->ReadD());
    $ctx->SendRETURN();
  }

  sub ServerSetup {
    my $ctx = shift;
    $ctx->ServiceCreate("MMUL",\&MMUL)
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    return $ctx;
  }

package main;

  our $srv = Net::PerlMsgque::FactoryAdd("mulserver", "MulServer")->New();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

