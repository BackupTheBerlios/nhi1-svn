#! /bin/bash
#+
#:  \file       sbin/SimpleHTTPServer.bash
#:  \brief      \$Id: LbMain 608 2013-02-07 09:31:46Z aotto1968 $
#:  
#:  (C) 2013 - NHI - #1 - Project - Group
#:  
#:  \version    \$Rev: 608 $
#:  \author     EMail: aotto1968 at users.berlios.de
#:  \attention  this software has GPL permissions to copy
#:              please contact AUTHORS for additional information
#:

id=SimpleHTTPServer

case $1 in
  start) 
    nohup python -m $id 8080 1>/tmp/$id.log 2>&1 &
    echo $! > /tmp/$id.pid
  ;;
  stop)
    kill $(</tmp/$id.pid)
  ;;
  log)
    test -r /tmp/$id.log && tail -f /tmp/$id.log
  ;;
  check)
    test -r /tmp/$id.pid && ps -eaf | grep $(</tmp/$id.pid)
  ;;
  *)
    echo "usage: $0 (start|stop|log|check)"
  ;;
esac

exit 0
