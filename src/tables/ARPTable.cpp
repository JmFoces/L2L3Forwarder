/*
 * ARPTable1.cpp
 *
 *  Created on: Jan 31, 2016
 *      Author: xshell
 */

#include <tables/ARPTable.h>

ARPTable* ARPTable::instance = NULL;

ARPTable* ARPTable::getInstance(){
	if(ARPTable::instance == NULL){
		ARPTable::instance = new ARPTable();
	}
	return instance;
}

ARPTable::ARPTable(){
	BOOST_LOG_TRIVIAL(debug) << "Created ARP TABLE";
}
ARPTable::~ARPTable(){

}

void ARPTable::insert(uint8_t* l3_addr, uint8_t* l2_addr,L3Socket* from_sock){
	//reordering integers
	//It's shifted to be clearer. Not really necessary since integers are identifiers.
	lock.lock();
	BOOST_LOG_TRIVIAL(trace)
				<< "ARP Insert/override " << format_mac_addr(l2_addr)
				<<"<-->" << format_ip_addr(l3_addr);
	arp_table[char_to_ip(l3_addr)] = new std::pair<uint64_t,L3Socket*>(
		char_to_mac((unsigned char*)l2_addr),
		from_sock
	);
	lock.unlock();
}
std::pair<uint64_t,L3Socket*>* ARPTable::query(uint8_t* l3_addr){
	lock.lock();
	std::pair<uint64_t,L3Socket*> *known_pair = NULL;
	//Warning this method doe snot shift ip bytes!!

	try{
		known_pair = arp_table.at(int8_ptr_to_uint32_shift(l3_addr));
		BOOST_LOG_TRIVIAL(debug)
			<< "ARP Match for " << format_ip_addr(l3_addr)
			<<"<-->" << format_mac_addr(&((uint8_t*)&known_pair->first)[0]);

	}catch (std::out_of_range &e){
		BOOST_LOG_TRIVIAL(debug)
			<< "No ARP Match for IP: " << format_ip_addr(l3_addr);
	}
	lock.unlock();
	return known_pair;
}
