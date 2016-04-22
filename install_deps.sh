#!/bin/bash
#Using Debian Stretch I've needed to install the following:
echo "Installing dependencies:"
apt-get install libboost-all-dev linux-headers-`uname -r | sed s/"\(.*\)-"//` gcc g++ make git
#Then you can just
echo "Downloading whole repo"
git clone https://github.com/jmfoces/L2L3Forwarder.git
cd L2L3Forwarder/Debug
make clean && make all
echo "Edit your config.json to setup the interfaces you want to use and GOOO!. Press Enter to launch it!"
read
./L2L3Forwarder
