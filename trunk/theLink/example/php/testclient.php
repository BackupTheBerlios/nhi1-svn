<?php
#+
#§  \file       theLink/example/php/testclient.php
#§  \brief      \$Id$
#§  
#§  (C) 2010 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

## setup the clients
$server = dirname($argv[0]) . "/testserver.php";
## create object
$c0 = new MqS();
$c00 = new MqS();
$c01 = new MqS();
$c000 = new MqS();
$c1 = new MqS();
$c10 = new MqS();
$c100 = new MqS();
$c101 = new MqS();
## setup object link
$c0->LinkCreate("c0",    "--srvname", "s0",  "--debug", getenv('TS_DEBUG'), "@", split(" ", getenv('PHP')), $server);
$c00->LinkCreateChild($c0,  "c00",   "--name", "c00", "--srvname", "s00");
$c01->LinkCreateChild($c0,  "c01",   "--name", "c01", "--srvname", "s01");
$c000->LinkCreateChild($c00, "c000",  "--name", "c000", "--srvname", "s000");
$c1->LinkCreate("c1",    "--srvname", "s1", array_slice($argv,1));
$c10->LinkCreateChild($c1,   "c10",   "--name", "c10", "--srvname", "s10");
$c100->LinkCreateChild($c10, "c100",  "--name", "c100", "--srvname", "s100");
$c101->LinkCreateChild($c10, "c101",  "--name", "c101", "--srvname", "s101");
## my helper
function Get($ctx) {
  $ret = "";
  $ctx->SendSTART();
  $ctx->SendEND_AND_WAIT("GTCX");
  $ret .= $ctx->ConfigGetName();
  $ret .= "+";
  $ret .= $ctx->ReadC();
  $ret .= $ctx->ReadC();
  $ret .= $ctx->ReadC();
  $ret .= $ctx->ReadC();
  $ret .= $ctx->ReadC();
  $ret .= $ctx->ReadC();
  $ret .= "\n";
  return $ret;
}
## do the tests
print(Get($c0));
print(Get($c00));
print(Get($c01));
print(Get($c000));
print(Get($c1));
print(Get($c10));
print(Get($c100));
print(Get($c101));
## do the cleanup
#$c0->Delete();
#$c1->Delete();

?>
