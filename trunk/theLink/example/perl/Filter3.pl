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

  sub ServerSetup {
    my $ctx = shift;
    my $ftr = $ctx->ServiceGetFilter();
    $ctx->ServiceProxy("+ALL");
    $ctx->ServiceProxy("+TRT");
    $ftr->ServiceProxy("+ALL");
    $ftr->ServiceProxy("+TRT");
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetName("Filter3");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetFactory(sub {new Filter3()});
    return $ctx;
  }


package main;

  our $srv = new Filter3();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

1;



