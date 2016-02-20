/*
 * RoutingTable.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: xshell
 */

#include <tables/RoutingTable.h>

RoutingTable* RoutingTable::instance = NULL;



RoutingTable* RoutingTable::getInstance(){
	if(RoutingTable::instance == NULL){
		RoutingTable::instance = new RoutingTable();
	}
	return instance;
}
bool RoutingTable::key_compare(uint64_t first, uint64_t second){
	return first > second;
}
RoutingTable::RoutingTable():routingtable(key_compare) {
	// Insert default route.

}

RoutingTable::~RoutingTable() {
	// TODO Auto-generated destructor stub
}

void RoutingTable::insert(
		in_addr_t mask,
		in_addr_t if_addr,
		L3Socket* output_sock,
		in_addr_t nexthop){
	//building key.
	BOOST_LOG_TRIVIAL(debug)
		<< "INSERT "
		<< "MASK: "
		<< format_ip_addr((uint8_t*)&mask)
		<< " NET: "
		<< format_ip_addr((uint8_t*)&if_addr);
	uint64_t key = 0;
	uint32_t* key_arr = (uint32_t*)&key; //Cast to 32bit int.
					//									 255     .255     .255     .0       .192     .168     .1       .0 (192.168.1.0/24)
	key_arr[0]=mask;//Set most important 32bits to mask. 11111111.11111111.11111111.00000000.11000000.10101000.00000001.00000000
	key_arr[1]=if_addr&mask; //Set less important 32bit to net.
	//Building nexthop and sock ip addr
	uint64_t next_hop_if_ip;
	uint32_t* next_hop_if_ip_arr = (uint32_t*)&next_hop_if_ip; //Cast to 32bit int.
	next_hop_if_ip_arr[0]=nexthop;
	next_hop_if_ip_arr[1]=if_addr;
	routingtable[key] = std::pair<L3Socket*,uint64_t>(output_sock,next_hop_if_ip);
	BOOST_LOG_TRIVIAL(debug)
		<< "INSERT "
		<< "KEY : "
		<< std::to_string(key)
		<< " as "
		<< format_ip_addr((uint8_t*)&key)
		<<","
		<<format_ip_addr((uint8_t*)&key_arr[1]);
}

std::pair<L3Socket*,uint64_t> RoutingTable::query(in_addr_t ip){
	//building key.
	std::pair<uint64_t,std::pair<L3Socket*,uint64_t>> component; // what a map<int, int> is made of
	BOOST_LOG_TRIVIAL(trace)<< "IP: "<<format_ip_addr((uint8_t*)&ip) << " AS num " << std::to_string(ip);
	BOOST_FOREACH(component, routingtable) {
		uint32_t *mask_net = (uint32_t*) &component.first;
		uint32_t filter = mask_net[0] & ip;

		BOOST_LOG_TRIVIAL(trace)<< "Query key "
				<< std::to_string(component.first)
		 	 	<< " MASK: "
				<< format_ip_addr((uint8_t*)&mask_net[0])
				<< " NET: "<< format_ip_addr((uint8_t*)&mask_net[1]);
		BOOST_LOG_TRIVIAL(trace)<< "FILTER : "
				<< format_ip_addr((uint8_t*)&filter)
				<< " VS MASK "
				<< format_ip_addr((uint8_t*)&mask_net[1]);
		if(filter == mask_net[1]){
			BOOST_LOG_TRIVIAL(trace)<< "Match found "
				<< std::to_string(component.first)
				<< " NEXTHOP: "<< format_ip_addr((uint8_t*)&component.second.second)
				<< " IF Addr: "<< format_ip_addr(&((uint8_t*)&component.second.second)[4]);
			return component.second;
		}
	}
	return std::pair<L3Socket*,in_addr_t>(NULL,0);
}
