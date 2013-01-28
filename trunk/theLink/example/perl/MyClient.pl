#+
#:  \file       theLink/example/perl/MyClient.pl
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

package main;

  our $ctx = new Net::PerlMsgque::MqS();
  eval {
    $ctx->ConfigSetName("MyClient");
    $ctx->LinkCreate(@ARGV);
    $ctx->SendSTART();
    $ctx->SendEND_AND_WAIT("HLWO");
    print $ctx->ReadC() . "\n";
  };
  if ($@) {
    $ctx->ErrorSet($@);
  }
  $ctx->Exit();


