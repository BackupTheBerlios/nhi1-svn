#+
#§  \file       theLink/example/perl/Filter4.pl
#§  \brief      \$Id: Filter4.pl 92 2009-12-21 11:58:07Z aotto1968 $
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev: 92 $
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

use strict;
use Net::PerlMsgque;

$| = 1;

package Filter4;
use base qw(Net::PerlMsgque::MqS);

  sub Event {
    my $ctx = shift;
    my $itms = $ctx->DictGet("itms");
    my $it = shift(@{$itms});
    if (defined($it)) {
      eval {
	my $ftr = $ctx->ServiceGetFilter();
	my ($token,$isTransaction,$bdy) = @{$it};
	$ftr->SendSTART();
	$ftr->SendBDY($bdy);
	if ($isTransaction) {
	  $ftr->SendEND_AND_WAIT($token);
	} else {
	  $ftr->SendEND($token);
	}
      };
      if ($@) {
	$ctx->ErrorSet($@);
	$ctx->ErrorPrint();
	$ctx->ErrorReset();
      }
    } else {
      $ctx->ErrorSetCONTINUE();
    }
  }

  sub FilterIn {
    my $ctx = shift;
    my $itms = $ctx->DictGet("itms");
    push(@{$itms},[$ctx->ServiceGetToken(), $ctx->ServiceIsTransaction(), $ctx->ReadBDY()]);
    $ctx->SendRETURN();
  }

  sub ServerSetup {
    my $ctx = shift;
    $ctx->ServiceCreate("+ALL", \&FilterIn);
    $ctx->DictSet("itms", []);
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetIgnoreExit(1);
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetEvent(\&Event);
    $ctx->ConfigSetFactory(sub {new Filter4()});
    return $ctx;
  }


package main;

  our $srv = new Filter4();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent({wait => "FOREVER"});
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

1;

