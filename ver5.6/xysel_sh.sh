#!/bin/sh
# set target pixel by XYSEL
# and then pulse pixel while capture scope out signals
# $1 command to issue "0 TRY5" or "0 TRY7"
# $2 wating time to complete TRY command.
#

export TRY=$1
export STM=$2
export SC_RUN="./dpo4104_noinit.exe"


x=0
while [ $x -ne 32 ]
do
	y=0
	while [ $y -ne 32 ]
	do
		echo "${x} ${y} XYSEL" | ./mupld_c.exe 
		echo "# ${x} ${y} XYSEL"
		#msleep 100
		msleep 25
		echo $TRY | ./mupld_c.exe 
		$SC_RUN
		msleep $STM
		y=`expr $y + 1`
	done
	x=`expr $x + 1`
done

exit

