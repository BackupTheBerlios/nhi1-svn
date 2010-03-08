#+
#§  \file       theLink/example/perl/manfilter.pl
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

sub FTRcmd {
  my $ctx = shift;
  my $ftr = $ctx->ServiceGetFilter();
  $ftr->SendSTART();
  while ($ctx->ReadItemExists()) {
    $ftr->SendC("<" . $ctx->ReadC() . ">");
  }
  $ftr->SendEND_AND_WAIT("+FTR");
  $ctx->SendRETURN();
}

package main;

  our $srv = new Net::PerlMsgque::MqS();
  eval {
    $srv->ConfigSetName("ManFilter");
    $srv->ConfigSetIsServer(1);
    $srv->ConfigSetFactory(sub {new Net::PerlMsgque::MqS()});
    $srv->LinkCreate(@ARGV);
    $srv->ServiceCreate("+FTR", \&FTRcmd);
    $srv->ServiceProxy("+EOF");
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();




