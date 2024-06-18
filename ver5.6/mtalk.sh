#!/bin/sh
LOGDIR="log/"
date2=`date +"%Y%m%d"`
echo " ++++++++++ MISC talk terminal +++++++++"
echo LOG file:  $LOGDIR$date2
echo "To quit, type :q"
echo


( ./mterm.exe -d | tee -a $LOGDIR$date2 ) &
sleep 3
./mcmdr.exe -s


