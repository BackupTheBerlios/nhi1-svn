#!/bin/bash
#+
#§  \file       theLink/tests/massclient.bash
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
NUM=$1;shift

rm -f /tmp/*.masstest.out

for (( i=1; i<=NUM; i++)) ; do
    echo "NUM = $i"
    $@ 1>/tmp/$i.masstest.out 2>&1 &
    #usleep 10000
done

let i--

while [[ ! -f /tmp/$i.masstest.out ]] ; do
  sleep 1
done

tail -f /tmp/$i.masstest.out

grep -i failed /tmp/*.masstest.out
