#+
#§  \file       theLink/example/perl/Filter2.pl
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
  die "my error";
}

package main;

  our $srv = new Net::PerlMsgque::MqS();
  eval {
    $srv->ConfigSetFilterFTR(\&FTRcmd);
    $srv->ConfigSetName("filter");
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();



