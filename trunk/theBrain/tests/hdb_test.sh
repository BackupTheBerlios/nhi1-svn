#+
#§  \file       theBrain/tests/hdb_test.sh
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
./tchtest write casket 50000 5000 5 5
./tchtest read casket
./tchtest remove casket
./tchtest write -mt -tl -td -rc 50 -xm 500000 casket 50000 5000 5 5
./tchtest read -mt -nb -rc 50 -xm 500000 casket
./tchtest remove -mt -rc 50 -xm 500000 casket
./tchtest write -as -tb -rc 50 -xm 500000 casket 50000 50000 5 5
./tchtest read -nl -rc 50 -xm 500000 casket
./tchtest remove -rc 50 -xm 500000 -df 5 casket
./tchtest rcat -pn 500 -xm 50000 -df 5 casket 50000 5000 5 5
./tchtest rcat -tl -td -pn 5000 casket 50000 500 5 15
./tchtest rcat -nl -pn 500 -rl casket 5000 500 5 5
./tchtest rcat -tb -pn 500 casket 5000 500 5 5
./tchtest rcat -ru -pn 500 casket 5000 500 1 1
./tchtest rcat -tl -td -ru -pn 500 casket 5000 500 1 1
./tchmgr list -pv casket > check.out
./tchmgr list -pv -fm 1 -px casket > check.out
./tchtest misc casket 5000
./tchtest misc -tl -td casket 5000
./tchtest misc -mt -tb casket 500
./tchtest wicked casket 50000
./tchtest wicked -tl -td casket 50000
./tchtest wicked -mt -tb casket 5000
./tchtest wicked -tt casket 5000
./tchtest wicked -tx casket 5000
./tchmttest write -xm 500000 -df 5 -tl casket 5 5000 500 5
./tchmttest read -xm 500000 -df 5 casket 5
./tchmttest read -xm 500000 -rnd casket 5
./tchmttest remove -xm 500000 casket 5
./tchmttest wicked -nc casket 5 5000
./tchmttest wicked -tl -td casket 5 5000
./tchmttest wicked -tb casket 5 5000
./tchmttest typical -df 5 casket 5 50000 5000
./tchmttest typical -rr 1000 casket 5 50000 5000
./tchmttest typical -tl -rc 50000 -nc casket 5 50000 5000
./tchmttest race -df 5 casket 5 10000
./tchmgr create casket 3 1 1
./tchmgr inform casket
./tchmgr put casket one first
./tchmgr put casket two second
./tchmgr put -dk casket three third
./tchmgr put -dc casket three third
./tchmgr put -dc casket three third
./tchmgr put -dc casket three third
./tchmgr put casket four fourth
./tchmgr put -dk casket five fifth
./tchmgr out casket one
./tchmgr out casket two
./tchmgr get casket three > check.out
./tchmgr get casket four > check.out
./tchmgr get casket five > check.out
./tchmgr list -pv casket > check.out
./tchmgr optimize casket
./tchmgr put -dc casket three third
./tchmgr get casket three > check.out
./tchmgr get casket four > check.out
./tchmgr get casket five > check.out
./tchmgr list -pv casket > check.out
rm -rf casket*
