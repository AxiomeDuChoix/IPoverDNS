#!/bin/bash
#Shut down tap0
#reset routes (polytechnique for ex.)

if [ "$#" -lt 1 ] ; then
	echo "Please indicate the DNS server's IP."
	exit 0
elif [ "$#" -gt 1 ] ; then
	echo "Too many arguments: Please only indicate the DNS server's IP."
	exit 1
fi

iptap=10.0.0.1

route del default gw $iptap

#get the origianl gateway
DEFR=$(netstat -rn | grep "^$1 " | awk '{print $2}')

route del $1

route add default gw $DEFR