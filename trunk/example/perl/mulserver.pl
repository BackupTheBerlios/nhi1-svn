#+
#§  \file       example/perl/mulserver.pl
#§  \brief      \$Id: mulserver.pl 507 2009-11-28 15:18:46Z dev1usr $
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

package mulserver;
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
    $ctx->ConfigSetName("mulserver");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetFactory(sub {new mulserver()});
    return $ctx;
  }

package main;

  our $srv = new mulserver();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



