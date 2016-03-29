/*
 * UserInterface.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: xshell
 */

#include "UserInterface.h"
#include <boost/foreach.hpp>
#include <sockets/SwitchSocket.h>
#include <sockets/L3Socket.h>
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
	boost::asio::io_service *io_service = new boost::asio::io_service;
	char op_mode = '0';
		std::list<SwitchSocket*> switch_ports;

		while(op_mode != OP_CODE_QUIT){
			std::cout << "Introduce the operation that you want to perform: \n a-> Add an interface \n t->Show ARP and routing tablesgm" << std::endl;
			std::cin >> op_mode;

			switch(op_mode){
				case OP_CODE_SHOW_TABLES:
					if (!SwitchSocket::bridge_table) {
						std::cout << "Not initialized. Add an interface first.";
						break;
					}
					break;
				case OP_CODE_ADD_IFACE:
					std::cout << " Please insert interface name" << std::endl;
					std::string if_name;
					std::cin >> if_name;
					L3Socket *new_port = new L3Socket(if_name,io_service);
					std::cout << " Please insert interface IP Address" << std::endl;
					std::string ip_addr;
					std::cin >> ip_addr;
					std::cout << " Please insert interface IP Netmask (255.255.255.0)" << std::endl;
					std::string ip_net;
					std::cin >> ip_net;
					new_port->add_ip_addr((unsigned char*) ip_addr.c_str(),(unsigned char*) ip_net.c_str());
					new_port->start();
					boost::thread *routing_thread = new boost::thread(&call_ios,io_service);
					routing_thread->detach();
					delete routing_thread;
					break;
			}
		}
		std::cout << " Bye!" << std::endl;
		io_service->stop();

		delete io_service;
		BOOST_FOREACH(SwitchSocket* sock, switch_ports){
			delete sock;
		}
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
	boost::thread* switch_thread;
	while(op_mode != OP_CODE_QUIT){
		std::cout << "Introduce the operation you want to perform: \n a-> Add an interface \n t->Show bridge table" << std::endl;
		std::cin >> op_mode;

		switch(op_mode){
			case OP_CODE_SHOW_TABLES:
				if (!SwitchSocket::bridge_table) {
					std::cout << "Not initialized. Add an interface first.";
					break;
				}
				std::cout << "Bridge table "  << SwitchSocket::get_bridge_table() << " Of size "<< SwitchSocket::get_bridge_table()->size()  << std::endl;
				for ( mac_table::const_iterator brtable_it = SwitchSocket::bridge_table->cbegin();
						brtable_it != SwitchSocket::bridge_table->cend(); brtable_it++){
					BOOST_LOG_TRIVIAL(debug) << "MAC:  " << brtable_it->first << " ON iface: " << brtable_it->second->ifname;
					std::cout << "MAC:  " << brtable_it->first << " ON iface: " << brtable_it->second->ifname<< std::endl;
				}
				break;
			case OP_CODE_ADD_IFACE:
				std::cout << " Please insert interface name" << std::endl;
				std::string if_name;
				std::cin >> if_name;
				SwitchSocket *new_port = new SwitchSocket(if_name,io_service);
				new_port->start();
				if(!switch_thread) switch_thread = new boost::thread(&call_ios,io_service);
				switch_thread->detach();
				delete switch_thread;
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
