/*
 * UserInterface.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: xshell
 */

#include "UserInterface.h"
#include <boost/foreach.hpp>
#include <sockets/SwitchSocket.h>
#include <cstddef>
#include <boost/thread.hpp>
UserInterface::UserInterface() {
	// TODO Auto-generated constructor stub

}

UserInterface::~UserInterface() {
	// TODO Auto-generated destructor stub
}

void UserInterface::run(){

	char op_mode = '0';
	while(true){
		std::cout << "Introduce the operation mode: \n r-> IPv4 Router \n s->Ethernet Switch";
		std::cin >> op_mode;
		switch(op_mode){
		case OP_MODE_ROUTER:
			run_router();
			break;
		case OP_MODE_SWITCH:
			run_switch();
			break;
		}
	}
}
void UserInterface::run_router(){
	std::cout << "Not implemented yet!!!" << std::endl;
	/*init_logging(boost::log::trivial::trace);
	boost::asio::io_service *io_service = new boost::asio::io_service;
	L3Socket vmnet1("vmnet1",io_service);
	L3Socket vmnet2("vmnet2",io_service);
	vmnet1.add_ip_addr((unsigned char*)"192.168.10.1",(unsigned char*)"255.255.255.0");
	vmnet2.add_ip_addr((unsigned char*)"192.168.11.1",(unsigned char*)"255.255.255.0");
	vmnet1.start();
	vmnet2.start();
	boost::thread* th0 = new boost::thread(&call_ios,io_service);
	boost::thread* th1 = new boost::thread(&call_ios,io_service);

	int stop;
	cin>> stop;
	io_service->stop();
	delete io_service;
	delete th0;
	delete th1;*/
}
void UserInterface::run_switch(){
	boost::asio::io_service *io_service = new boost::asio::io_service;
	char op_mode = '0';
	std::list<SwitchSocket*> switch_ports;

	while(op_mode != OP_CODE_QUIT){
		std::cout << "Introduce the operation you want to perform: \n a-> Add an interface \n t->Show bridge table" << std::endl;
		std::cin >> op_mode;
		switch(op_mode){
			case OP_CODE_SHOW_TABLES:
				for (mac_table::iterator mac_iface_pair=bridge_table.begin();mac_iface_pair!=bridge_table.end();mac_iface_pair++){
					std::cout << "MAC " << format_mac_addr((uint8_t*) &mac_iface_pair->first);
					std::cout << " On port " << mac_iface_pair->second->ifname;
					std::cout << std::endl;
				}
				break;
			case OP_CODE_ADD_IFACE:
				std::cout << " Please insert interface name" << std::endl;
				std::string if_name;
				std::cin >> if_name;
				SwitchSocket *new_port = new SwitchSocket(if_name,io_service);
				new_port->start();
				boost::thread* th0 = new boost::thread(&call_ios,io_service);
				th0->detach();
				delete th0;

				break;
		}
	}
	std::cout << " Bye!" << std::endl;
	io_service->stop();

	delete io_service;
	BOOST_FOREACH(SwitchSocket* sock, switch_ports){
		delete sock;
	}

/*
	SwitchSocket eth1("vmnet1",io_service);
	SwitchSocket eth2("vmnet2",io_service);

	eth1.start();
	eth2.start();

	boost::thread* th0 = new boost::thread(&call_ios,io_service);
	boost::thread* th1 = new boost::thread(&call_ios,io_service);
	int stop;
	cin>> stop;
	io_service->stop();
	delete io_service;
	delete th0;
	delete th1;*/
}
