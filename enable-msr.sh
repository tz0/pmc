#!/bin/bash

#enable and make performance counters available, run it with sudo
modprobe msr
echo "2" > /sys/devices/cpu/rdpmc
