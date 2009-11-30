#+
#§  \file       example/perl/Filter1.pl
#§  \brief      \$Id: Filter1.pl 507 2009-11-28 15:18:46Z dev1usr $
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

our @data = ();

sub FTRcmd {
  my $ctx = shift;
  my @L = ();
  while ($ctx->ReadItemExists()) {
    push(@L, "<" . $ctx->ReadC() . ">");
  }
  push(@data,\@L);
}

sub EOFcmd {
  my $ctx = shift;
  foreach (@data) {
    $ctx->SendSTART();
    foreach (@$_) {
      $ctx->SendC($_);
    }
    $ctx->SendFTR();
  }
}

package main;

  our $srv = new Net::PerlMsgque::MqS();
  eval {
    $srv->ConfigSetFilterFTR(\&FTRcmd);
    $srv->ConfigSetFilterEOF(\&EOFcmd);
    $srv->ConfigSetName("filter");
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



