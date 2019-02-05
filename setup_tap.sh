#!/bin/bash
# Creates a tap interface: tap0
# Modifies the routing table so that everything goes through Tap0 except stuff going to the DNS server.

if [ "$#" -lt 1 ] ; then
	echo "Please indicate the DNS server's IP."
	exit 0
elif [ "$#" -gt 1 ] ; then
	echo "Too many arguments: Please only indicate the DNS server's IP."
	exit 1
fi

iptap=10.0.0.1

#creates a tap interface
openvpn --mktun --dev tap0

#bring it up
ip link set tap0 up

#gives it an ip address
ip addr add $iptap dev tap0

#gets the default gateway (IP address of the router)
DEFR=$(netstat -rn | grep "^0.0.0.0 " | awk '{print $2}')

#deletes it
route delete default gw $DEFR

#only if we want to reach the DNS server we pass by the original gateway 
route add -host $1 gw $DEFR

#assign tap0 as the default gateway: all traffic goes into tap0 first
route add default gw $iptap