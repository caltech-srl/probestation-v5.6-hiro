#!/bin/sh
# set target pixel by XYSEL
# and then pulse pixel while capture scope out signals
#
# ./single_xysel.sh 1 TRY5 10 12

export NTRY=$1
export TRY=$2
export SC_RUN="./dpo4104_noinit.exe"
export x=$3
export y=$4


echo "${x} ${y} XYSEL" | ./mupld_c.exe 
echo ":P " | ./mupld_c.exe 
echo "# ${x} ${y} XYSEL"
# msleep 100
sleep 0.1

echo $NTRY $TRY | ./mupld_c.exe 
# msleep 20
sleep 0.02
$SC_RUN


exit

