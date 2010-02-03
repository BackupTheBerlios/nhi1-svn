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
use FileHandle;

$| = 1;

package Filter4;
use base qw(Net::PerlMsgque::MqS);

  sub ErrorWrite {
    my $ftr = shift;
    my $FH = $ftr->DictGet("FH");
    print($FH "ERROR: " . $ftr->ErrorGetText() . "\n");
    $ftr->ErrorReset();
  }

  sub WRIT {
    my $ftr = shift;
    my $FH = $ftr->DictGet("FH");
    print($FH $ftr->ReadC() . "\n");
    $ftr->SendRETURN();
  }

  sub EXIT {
    my $ctx = shift;
    $ctx->Exit;
  }

  sub LOGF {
    my $ctx = shift;
    my $ftr = $ctx->ServiceGetFilter();
    my $file = $ctx->ReadC();
    if ($ftr->LinkGetTargetIdent() eq "transFilter") {
      $ftr->SendSTART();
      $ftr->SendC($file);
      $ftr->SendEND_AND_WAIT("LOGF");
    } else {
      open(my $FH, ">>", $file);
      $FH->autoflush(1);
      $ftr->DictSet("FH", $FH);
    }
    $ctx->SendRETURN();
  }

  sub Event {
    my $ctx = shift;
    my $itms = $ctx->DictGet("itms");
    my $it = @{$itms}[0];
    if (!defined($it)) {
      $ctx->ErrorSetCONTINUE();
    } else {
      my $ftr = $ctx->ServiceGetFilter();
      eval {
	my ($token,$isTransaction,$bdy) = @{$it};
	$ftr->LinkConnect();
	$ftr->SendSTART();
	$ftr->SendBDY($bdy);
	if ($isTransaction) {
	  $ftr->SendEND_AND_WAIT($token);
	} else {
	  $ftr->SendEND($token);
	}
      };
      if ($@) {
	$ftr->ErrorSet($@);
	if ($ftr->ErrorIsEXIT) {
	  $ftr->ErrorReset();
	  return;
	} else {
	  $ftr->ErrorWrite();
	}
      }
      shift(@{$itms});
    }
  }

  sub FilterIn {
    my $ctx = shift;
    my $itms = $ctx->DictGet("itms");
    push(@{$itms},[$ctx->ServiceGetToken(), $ctx->ServiceIsTransaction(), $ctx->ReadBDY()]);
    $ctx->SendRETURN();
  }

  sub ServerCleanup {
    my $ctx = shift;
    my $FH = $ctx->DictGet("FH");
    if (defined($FH)) {
      close($FH);
    }
  }

  sub ServerSetup {
    my $ctx = shift;
    $ctx->ServiceCreate("+ALL", \&FilterIn);
    $ctx->ServiceCreate("LOGF", \&LOGF);
    $ctx->ServiceCreate("EXIT", \&EXIT);
    $ctx->ServiceGetFilter()->ServiceCreate("WRIT", \&WRIT);
    $ctx->DictSet("itms", []);
    $ctx->DictSet("FH", undef);
  }

  sub new {
    my $class = shift;
    my $ctx = $class->SUPER::new(@_);
    $ctx->ConfigSetIgnoreExit(1);
    $ctx->ConfigSetIdent("transFilter");
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetServerCleanup(\&ServerCleanup);
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

