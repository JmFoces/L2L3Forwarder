# L2L3Forwarder
This project started at  01/2016.
The main objective is to give a simple, not performance oriented, implementation of an Ethernet switch and an IPv4 router. 

2016/02/20
	Sources upgraded.
	There's a lot of work pending.
	In the following days I will write some comments and reorganize the whole code, some parts maybe need to be reworked.
	The main will be reworked at all to include a configuration file.
	More ideas will arise so I'll be writting them here.
	
2016/04/22 Build Install instructions.
	Now It's possible to use Eclipse's Egit plugin to directly integrate the project in another development system.
	Using Debian Stretch I've needed to install the following:
	apt-get install libboost-all-dev linux-headers-`uname -r | sed s/"\(.*\)-"//` gcc g++ make git
	Then you can just
	git clone https://github.com/jmfoces/L2L3Forwarder.git
	cd L2L3Forwarder/Debug
	make clean && make all
	Edit your config.json to setup the interfaces you want to use and GOOO!
	./L2L3Forwarder
	
	