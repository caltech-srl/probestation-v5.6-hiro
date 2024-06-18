#!/bin/bash
#
#  kill all mterm and mcmdr 


misc=$(ps -s | grep mterm | awk '{print $1}')
if [ "$misc" != "" ]; then
	kill $misc
fi

misc=$(ps -s | grep mcmdr | awk '{print $1}')
if [ "$misc" != "" ]; then
	kill $misc
fi

exit

