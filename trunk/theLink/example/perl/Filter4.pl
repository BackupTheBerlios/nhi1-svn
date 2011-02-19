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
    my $ctx = shift;
    my $FH = $ctx->DictGet("FH");
    if (defined($FH)) {
      print $FH "ERROR: " . $ctx->ErrorGetText() . "\n";
    } else {
      $ctx->Log("ErrorWrite", 0, $ctx->ErrorGetText() . "\n");
    }
    $ctx->ErrorReset();
  }

  sub WRIT {
    my $ftr = shift;
    my $FH = $ftr->ServiceGetFilter()->DictGet("FH");
    if (defined($FH)) {
      print $FH $ftr->ReadC() . "\n";
    } else {
      $ftr->Log("ErrorWrite", 0, $ftr->ReadC() . "\n");
    }
    $ftr->SendRETURN();
  }

  sub EXIT {
    exit 1;
  }

  sub LOGF {
    my $ctx = shift;
    my $ftr = $ctx->ServiceGetFilter();
    if ($ftr->LinkGetTargetIdent() eq "transFilter") {
      $ctx->ReadForward($ftr);
    } else {
      open(my $FH, ">>", $ctx->ReadC());
      $FH->autoflush(1);
      $ctx->DictSet("FH", $FH);
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
      eval {
	my $ftr = $ctx->ServiceGetFilter();
	$ftr->LinkConnect();
	$ctx->ReadLOAD($it);
	$ctx->ReadForward($ftr);
      };
      if ($@) {
	$ctx->ErrorSet($@);
	if ($ctx->ErrorIsEXIT) {
	  $ctx->ErrorReset();
	  return;
	} else {
	  $ctx->ErrorWrite();
	}
      }
      shift(@{$itms});
    }
  }

  sub FilterIn {
    my $ctx = shift;
    my $itms = $ctx->DictGet("itms");
    push(@{$itms}, $ctx->ReadDUMP());
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
    my $ftr = $ctx->ServiceGetFilter();
    $ctx->ServiceCreate("+ALL", \&FilterIn);
    $ctx->ServiceCreate("LOGF", \&LOGF);
    $ctx->ServiceCreate("EXIT", \&EXIT);
    $ftr->ServiceCreate("WRIT", \&WRIT);
    $ftr->ServiceProxy("+TRT");
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

