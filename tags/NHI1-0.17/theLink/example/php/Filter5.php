<?php
#+
#:  \file       theLink/example/php/Filter5.php
#:  \brief      \$Id$
#:  
#:  (C) 2011 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev$
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

// F1 *******************************************************

class F1 extends MqS implements iServerSetup {
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceProxy('+EOF');
  }
  public function FTRcmd() {
    $ftr = $this->ServiceGetFilter();
    $ftr->SendSTART();
    $ftr->SendC('F1');
    $ftr->SendC($ftr->ConfigGetName());
    $ftr->SendI($ftr->ConfigGetStartAs());
    $ftr->SendI($this->ConfigGetStatusIs());
    while ($this->ReadItemExists()) {
      $ftr->SendC($this->ReadC());
    }
    $ftr->SendEND_AND_WAIT('+FTR');
    $this->SendRETURN();
  }
}

// F2 *******************************************************

class F2 extends MqS implements iServerSetup {
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceProxy('+EOF');
  }
  public function FTRcmd() {
    $ftr = $this->ServiceGetFilter();
    $ftr->SendSTART();
    $ftr->SendC('F2');
    $ftr->SendC($ftr->ConfigGetName());
    $ftr->SendI($ftr->ConfigGetStartAs());
    $ftr->SendI($this->ConfigGetStatusIs());
    while ($this->ReadItemExists()) {
      $ftr->SendC($this->ReadC());
    }
    $ftr->SendEND_AND_WAIT('+FTR');
    $this->SendRETURN();
  }
}

// F3 *******************************************************

class F3 extends MqS implements iServerSetup {
  public function ServerSetup() {
    $this->ServiceCreate('+FTR', array(&$this, 'FTRcmd'));
    $this->ServiceProxy('+EOF');
  }
  public function FTRcmd() {
    $ftr = $this->ServiceGetFilter();
    $ftr->SendSTART();
    $ftr->SendC('F3');
    $ftr->SendC($ftr->ConfigGetName());
    $ftr->SendI($ftr->ConfigGetStartAs());
    $ftr->SendI($this->ConfigGetStatusIs());
    while ($this->ReadItemExists()) {
      $ftr->SendC($this->ReadC());
    }
    $ftr->SendEND_AND_WAIT('+FTR');
    $this->SendRETURN();
  }
}

// Main ******************************************************

FactoryAdd('F1');
FactoryAdd('F2');
FactoryAdd('F3');

#fputs(STDERR, "111111111111111 -> " . join(", ", $argv) . "\n");

# first argument (the script-name) is not needed
array_shift($argv);

$srv = FactoryGetCalled($argv[0])->New();
try {
  $srv->LinkCreate($argv);
  $srv->ProcessEvent(MqS::WAIT_FOREVER);
} catch (Exception $ex) {
  $srv->ErrorSet($ex);
}
$srv->Exit();

?>

