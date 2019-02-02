#!/bin/bash

ip link set tap0 up
ip addr add 10.0.0.1/24 dev tap0
