#+
#§  \file       theLink/example/perl/mulclient.pl
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

use strict;
use Net::PerlMsgque;

package main;

  our $ctx = new Net::PerlMsgque::MqS();
  eval {
    $ctx->ConfigSetName("MyMul");
    $ctx->LinkCreate(@ARGV);
    $ctx->SendSTART();
    $ctx->SendD(3.67);
    $ctx->SendD(22.3);
    $ctx->SendEND_AND_WAIT("MMUL");
    print $ctx->ReadD() . "\n";
  };
  if ($@) {
    $ctx->ErrorSet($@);
  }
  $ctx->Exit();




