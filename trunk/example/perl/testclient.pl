#+
#§  \file       example/perl/testclient.pl
#§  \brief      \$Id: testclient.pl 507 2009-11-28 15:18:46Z dev1usr $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 507 $
#§  \author     EMail: aotto1968 at users.sourceforge.net
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

#use strict;
use Net::PerlMsgque;
use File::Spec::Functions;
use File::Basename;

package main;

  ## setup the clients
  our $server = catfile (dirname($0), "testserver.pl");
  ## create object
  our $c0    = new Net::PerlMsgque::MqS();
  our $c00   = new Net::PerlMsgque::MqS();
  our $c01   = new Net::PerlMsgque::MqS();
  our $c000  = new Net::PerlMsgque::MqS();
  our $c1    = new Net::PerlMsgque::MqS();
  our $c10   = new Net::PerlMsgque::MqS();
  our $c100  = new Net::PerlMsgque::MqS();
  our $c101  = new Net::PerlMsgque::MqS();
  ## my helper
  sub Get {
    my $ctx = shift;
    my $RET = "";
    $ctx->SendSTART();
    $ctx->SendEND_AND_WAIT("GTCX");
    $RET .= $ctx->ConfigGetName();
    $RET .= "+";
    $RET .= $ctx->ReadC();
    $RET .= $ctx->ReadC();
    $RET .= $ctx->ReadC();
    $RET .= $ctx->ReadC();
    $RET .= $ctx->ReadC();
    $RET .= $ctx->ReadC();
    return $RET;
  }
  eval {
    ## setup object link
    $c0->LinkCreate	    (      "--name", "c0",   "--srvname", "s0",  "--debug", $ENV{"TS_DEBUG"}, "@", "perl", $server);
    $c00->LinkCreateChild   ($c0,  "--name", "c00",  "--srvname", "s00");
    $c01->LinkCreateChild   ($c0,  "--name", "c01",  "--srvname", "s01");
    $c000->LinkCreateChild  ($c00, "--name", "c000", "--srvname", "s000");
    $c1->LinkCreate	    (      "--name", "c1",   "--srvname", "s1", @ARGV);
    $c10->LinkCreateChild   ($c1,  "--name", "c10",  "--srvname", "s10");
    $c100->LinkCreateChild  ($c10, "--name", "c100", "--srvname", "s100");
    $c101->LinkCreateChild  ($c10, "--name", "c101", "--srvname", "s101");
    ## do the tests
    print Get($c0)    . "\n";
    print Get($c00)   . "\n";
    print Get($c01)   . "\n";
    print Get($c000)  . "\n"; 
    print Get($c1)    . "\n"; 
    print Get($c10)   . "\n"; 
    print Get($c100)  . "\n"; 
    print Get($c101)  . "\n"; 
  };
  if ($@) {
    $c0->ErrorSet ($@);
  }
  $c0->Exit();


