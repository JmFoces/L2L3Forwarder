#!/bin/bash
#Using Debian Stretch I've needed to install the following:
echo "Installing dependencies:"
apt-get install libboost-all-dev linux-headers-`uname -r | sed s/"\(.*\)-"//` gcc g++ make git
