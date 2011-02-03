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
    if (defined($FH)) {
      print $FH "ERROR: " . $ftr->ErrorGetText() . "\n";
    } else {
      $ftr->Log("ErrorWrite", 0, $ftr->ErrorGetText() . "\n");
    }
    $ftr->ErrorReset();
  }

  sub WRIT {
    my $ftr = shift;
    my $FH = $ftr->DictGet("FH");
    if (defined($FH)) {
      print $FH $ftr->ReadC() . "\n";
    } else {
      $ftr->Log("ErrorWrite", 0, $ftr->ReadC() . "\n");
    }
    $ftr->SendRETURN();
  }

  sub EXIT {
    my $ctx = shift;
    $ctx->ErrorSetEXIT();
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
	$ftr->LinkConnect();
	$ftr->SendBDY($it);
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
    push(@{$itms}, $ctx->ReadBDY());
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
    $ctx->ConfigSetServerSetup(\&ServerSetup);
    $ctx->ConfigSetServerCleanup(\&ServerCleanup);
    $ctx->ConfigSetEvent(\&Event);
    bless $ctx, $class;
    return $ctx;
  }


package main;

  Net::PerlMsgque::MqFactoryS::Default("transFilter", "Filter4");
  our $srv = new Filter4();
  eval {
    $srv->LinkCreate(@ARGV);
    $srv->ProcessEvent(Net::PerlMsgque::WAIT_FOREVER);
  };
  if ($@) {
    $srv->ErrorSet($@);
  }
  $srv->Exit();

1;



