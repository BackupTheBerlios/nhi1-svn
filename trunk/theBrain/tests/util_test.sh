#+
#§  \file       theBrain/tests/util_test.sh
#§  \brief      \$Id$
#§  
#§  (C) 2007 - NHI - #1 - Project - Group
#§  
#§  \version    \$Rev$
#§  \author     EMail: aotto1968 at users.berlios.de
#§  \attention  this software has GPL permissions to copy
#§              please contact AUTHORS for additional information
#§

rm -rf casket*
./tcamgr version
./tcutest xstr 50000
./tcutest list -rd 50000
./tcutest map -rd -tr 50000
./tcutest map -rd -tr -rnd -dc 50000
./tcutest tree -rd -tr 50000
./tcutest tree -rd -tr -rnd -dc 50000
./tcutest mdb -rd -tr 50000
./tcutest mdb -rd -tr -rnd -dc 50000
./tcutest mdb -rd -tr -rnd -dpr 50000
./tcutest ndb -rd -tr 50000
./tcutest ndb -rd -tr -rnd -dc 50000
./tcutest ndb -rd -tr -rnd -dpr 50000
./tcutest misc 500
./tcutest wicked 50000
./tcumttest combo 5 50000 500
./tcumttest combo -rnd 5 50000 500
./tcumttest typical 5 50000 5000
./tcumttest typical -rr 1000 5 50000 5000
./tcumttest typical -nc 5 50000 5000
./tcumttest combo -tr 5 50000 500
./tcumttest combo -tr -rnd 5 50000 500
./tcumttest typical -tr 5 50000 5000
./tcumttest typical -tr -rr 1000 5 50000 5000
./tcumttest typical -tr -nc 5 50000 5000
./tcucodec url Makefile > check.in
./tcucodec url -d check.in > check.out
./tcucodec base Makefile > check.in
./tcucodec base -d check.in > check.out
./tcucodec quote Makefile > check.in
./tcucodec quote -d check.in > check.out
./tcucodec mime Makefile > check.in
./tcucodec mime -d check.in > check.out
./tcucodec pack -bwt Makefile > check.in
./tcucodec pack -d -bwt check.in > check.out
./tcucodec tcbs Makefile > check.in
./tcucodec tcbs -d check.in > check.out
./tcucodec zlib Makefile > check.in
./tcucodec zlib -d check.in > check.out
./tcucodec xml Makefile > check.in
./tcucodec xml -d check.in > check.out
./tcucodec cstr Makefile > check.in
./tcucodec cstr -d check.in > check.out
./tcucodec ucs Makefile > check.in
./tcucodec ucs -d check.in > check.out
./tcucodec date -ds '1978-02-11T18:05:30+09:00' -rf > check.out
./tcucodec cipher -key "mikio" Makefile > check.in
./tcucodec cipher -key "mikio" check.in > check.out
./tcucodec tmpl -var name mikio -var nick micky \
  '@name=[%name%][%IF nick%] nick=[%nick%][%END%][%IF hoge%][%ELSE%].[%END%]' > check.out
./tcucodec conf > check.out
rm -rf casket*

