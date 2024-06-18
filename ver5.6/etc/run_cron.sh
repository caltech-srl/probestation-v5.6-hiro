#!/bin/sh
# Shell script to run DMM (Agilent 34410A) every miniute.
# for direct USB connection, use read-usb.sh
#   this will call a34410_2.exe which original source programs
#   are in work/agilent/IVI/a34410_2 directory. 
#   this is VC++ base program. to compile user need VS.net.
# for GPIB-USB connection, use read-gpib.sh
#   this will call dmmtest.exe which an original source program
#   is in work/gpib-usb/lscan_1/source/dmmtest.c
#   this is CYGWIN/gcc base program.
# 09/04/08 hiro

# for reading 34410A by direct USB connection
###export DMMCOM='./read-usb.sh'
# for reading 34410A by NS GPIB-USB-HS
export DMMCOM='./read-gpib.sh'

$DMMCOM
while [ 1 ]; do
   export date1=`date +"%x %H:%M"`
   export date2=`date +"%x %H:%M"`
   if  [ "$date1" = "$date2" ]; then
#      echo "not yet " $date1 $date2
       sleep 58
   fi
   while [ "$date1" = "$date2" ]; do
#      echo "still not yet" $date1 $date2
       export date2=`date +"%x %H:%M"`
   done
#  echo "move !!!! " $date1 $date2
   $DMMCOM
done

exit
