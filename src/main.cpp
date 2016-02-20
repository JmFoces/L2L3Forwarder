/*
 * main.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */

#include <cstddef>
#include <iostream>
#include <map>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <sockets/L3Socket.h>
#include <sockets/SwitchSocket.h>
#include <tables/RoutingTable.h>
using namespace std;
void init_logging()
{
    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= boost::log::trivial::trace
    );
}
void call_ios(boost::asio::io_service* ios){
	ios->run();
}

void runSwitch(){
	boost::asio::io_service *io_service = new boost::asio::io_service;

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
	delete th1;

}
void runARPResponder(){
	init_logging();
	boost::asio::io_service *io_service = new boost::asio::io_service;
	L3Socket vmnet1("vmnet1",io_service);
	L3Socket vmnet2("vmnet2",io_service);
	/*ARPProtocolHandler* arp_hdl = ARPProtocolHandler::getInstance();
	 vmnet1.add_protocol(arp_hdl); //OLD add protocol callorder now add ip calls this.
	 */
	vmnet1.add_ip_addr((unsigned char*)"192.168.10.12",(unsigned char*)"255.255.255.0");
	vmnet1.start();
	vmnet2.start();
	boost::thread* th0 = new boost::thread(&call_ios,io_service);
	int stop;
	cin>> stop;
	io_service->stop();
	delete io_service;
	delete th0;

}
/*
 * Pending update to 64bit nexthop

void test_rt_table_insert(const char* mask_c,const char* net_c,const  char* next_hop_c,RoutingTable* rt_table){
	in_addr mask;
	in_addr net;
	in_addr next_hop;
	inet_aton(net_c,&net);
	inet_aton(mask_c,&mask);
	inet_aton(next_hop_c,&next_hop);
	rt_table->insert(mask.s_addr,net.s_addr,NULL,next_hop.s_addr);
}
void routing_table_test(){RoutingTable* rt_table = RoutingTable::getInstance();
	test_rt_table_insert("255.0.0.0","192.168.1.0","0.0.0.0",rt_table);
	test_rt_table_insert("255.255.0.0","192.168.1.0","0.0.0.1",rt_table);
	test_rt_table_insert("255.255.255.0","192.168.1.0","0.0.0.2",rt_table);
	test_rt_table_insert("255.255.255.254","192.168.1.0","0.0.0.3",rt_table);
	test_rt_table_insert("255.255.255.252","192.168.1.0","0.0.0.4",rt_table);
	test_rt_table_insert("255.255.255.248","192.168.1.0","0.0.0.5",rt_table);
	test_rt_table_insert("255.255.255.240","192.168.1.0","0.0.0.6",rt_table);
	test_rt_table_insert("255.255.255.224","192.168.1.0","0.0.0.7",rt_table);
	test_rt_table_insert("255.255.255.192","192.168.1.0","0.0.0.8",rt_table);
	test_rt_table_insert("255.255.255.128","192.168.1.0","0.0.0.9",rt_table);

	in_addr query;
	inet_aton("192.168.1.1",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.2",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.5",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.10",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.25",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.60",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.120",&query);
	rt_table->query(query.s_addr);
	inet_aton("192.168.1.252",&query);
	rt_table->query(query.s_addr);
}
*/
int main(){

	init_logging();
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
	delete th1;

}
