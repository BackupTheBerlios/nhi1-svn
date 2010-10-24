#+
#§  \file       theBrain/tests/bdb_test.sh
#§  \brief      \$Id$
#§  
#§  (C) 2009 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§
rm -rf casket*
./tcbtest write casket 50000 5 5 5000 5 5
./tcbtest read casket
./tcbtest remove casket
./tcbmgr list -rb 00001000 00002000 casket > check.out
./tcbmgr list -fm 000001 casket > check.out
./tcbtest write -mt -tl -td -ls 1024 casket 50000 5000 5000 5000 5 5
./tcbtest read -mt -nb casket
./tcbtest remove -mt casket
./tcbtest write -tb -xm 50000 casket 50000 5 5 50000 5 5
./tcbtest read -nl casket
./tcbtest remove -df 5 casket
./tcbtest rcat -lc 5 -nc 5 -df 5 -pn 500 casket 50000 5 5 5000 5 5
./tcbtest rcat -tl -td -pn 5000 casket 50000 5 5 500 5 15
./tcbtest rcat -nl -pn 5000 -rl casket 15000 5 5 500 5 5
./tcbtest rcat -ca 1000 -tb -pn 5000 casket 15000 5 5 500 5 5
./tcbtest rcat -ru -pn 500 casket 5000 5 5 500 1 1
./tcbtest rcat -cd -tl -td -ru -pn 500 casket 5000 5 5 500 1 1
./tcbmgr list -pv casket > check.out
./tcbtest queue casket 15000 5 5
./tcbtest misc casket 5000
./tcbtest misc -tl -td casket 5000
./tcbtest misc -mt -tb casket 500
./tcbtest wicked casket 50000
./tcbtest wicked -tl -td casket 50000
./tcbtest wicked -mt -tb casket 5000
./tcbtest wicked -tt casket 5000
./tcbtest wicked -tx casket 5000
./tcbtest write -cd -lc 5 -nc 5 casket 5000 5 5 5 5 5
./tcbtest read -cd -lc 5 -nc 5 casket
./tcbtest remove -cd -lc 5 -nc 5 casket
./tcbmgr list -pv casket > check.out
./tcbtest write -ci -td -lc 5 -nc 5 casket 5000 5 5 5 5 5
./tcbtest read -ci -lc 5 -nc 5 casket
./tcbtest remove -ci -lc 5 -nc 5 casket
./tcbmgr list -pv casket > check.out
./tcbtest write -cj -tb -lc 5 -nc 5 casket 5000 5 5 5 5 5
./tcbtest read -cj -lc 5 -nc 5 casket
./tcbtest remove -cj -lc 5 -nc 5 casket
./tcbmgr list -pv casket > check.out
./tcbmttest write -df 5 -tl casket 5 5000 5 5 500 5
./tcbmttest read -df 5 casket 5
./tcbmttest read -rnd casket 5
./tcbmttest remove casket 5
./tcbmttest wicked -nc casket 5 5000
./tcbmttest wicked -tl -td casket 5 5000
./tchmttest wicked -tb casket 5 5000
./tcbmttest typical -df 5 casket 5 50000 5 5
./tcbmttest typical -rr 1000 casket 5 50000 5 5
./tcbmttest typical -tl -nc casket 5 50000 5 5
./tcbmttest race -df 5 casket 5 10000
./tcbmgr create casket 4 4 3 1 1
./tcbmgr inform casket
./tcbmgr put casket one first
./tcbmgr put casket two second
./tcbmgr put -dk casket three third
./tcbmgr put -dc casket three third
./tcbmgr put -dc casket three third
./tcbmgr put -dd casket three third
./tcbmgr put -dd casket three third
./tcbmgr put casket four fourth
./tcbmgr put -dk casket five fifth
./tcbmgr out casket one
./tcbmgr out casket two
./tcbmgr get casket three > check.out
./tcbmgr get casket four > check.out
./tcbmgr get casket five > check.out
./tcbmgr list -pv casket > check.out
./tcbmgr list -j three -pv casket > check.out
./tcbmgr optimize casket
./tcbmgr put -dc casket three third
./tcbmgr get casket three > check.out
./tcbmgr get casket four > check.out
./tcbmgr get casket five > check.out
./tcbmgr list -pv casket > check.out
rm -rf casket*
rm -f check.out check.in
