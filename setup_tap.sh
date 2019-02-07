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

#create a tap interface
openvpn --mktun --dev tap0

#bring it up
ip link set tap0 up

#give it an ip address
ip addr add $iptap/24 dev tap0

#Limits the MSS to the interface's MTU
iptables -I OUTPUT -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu

#get the default gateway (IP address of the router)
DEFR=$(netstat -rn | grep "^0.0.0.0 " | awk '{print $2}')

#delete it
route del default gw $DEFR
route del 129.104.32.251 gw $DEFR

#only if we want to reach the DNS server we pass by the original gateway 
route add -host $1 gw $DEFR

#assign tap0 as the default gateway: all traffic goes into tap0 first
route add default gw $iptap

#show the IP routing table
netstat -rn
