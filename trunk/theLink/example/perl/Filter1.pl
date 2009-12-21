#+
#§  \file       theLink/example/perl/Filter1.pl
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

our @data = ();

sub FTRcmd {
  my $ctx = shift;
  my @L = ();
  while ($ctx->ReadItemExists()) {
    push(@L, "<" . $ctx->ReadC() . ">");
  }
  push(@data,\@L);
  $ctx->SendRETURN();
}

sub EOFcmd {
  my $ctx = shift;
  my $ftr = $ctx->ConfigGetFilter();
  foreach (@data) {
    $ftr->SendSTART();
    foreach (@$_) {
      $ftr->SendC($_);
    }
    $ftr->SendEND_AND_WAIT("+FTR");
  }
  $ftr->SendSTART();
  $ftr->SendEND_AND_WAIT("+EOF");
  $ctx->SendRETURN();
}

package main;

  our $srv = new Net::PerlMsgque::MqS();
  eval {
    $srv->ConfigSetName("filter");
    $srv->ConfigSetIsServer(1);
    $srv->ConfigSetFactory(sub {new Net::PerlMsgque::MqS()});
    $srv->LinkCreate(@ARGV);
    $srv->ServiceCreate("+FTR", \&FTRcmd);
    $srv->ServiceCreate("+EOF", \&EOFcmd);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



