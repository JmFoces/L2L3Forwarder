/*
 * SwitchSocket.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#include <sockets/SwitchSocket.h>
list<SwitchSocket*> SwitchSocket::brothers;
mac_table* SwitchSocket::bridge_table = NULL;

SwitchSocket::SwitchSocket(string ifname,boost::asio::io_service* io_service):
	L2Socket(ifname,io_service) {
	if (!bridge_table) bridge_table = new mac_table();
	brothers.push_back(this);
}

SwitchSocket::~SwitchSocket() {

}

uint64_t SwitchSocket::get_destination_mac(const char* packet){

	//Allocating space for temporal MAC.
	char *data = (char*) malloc(6*sizeof(char));
	//Copying data to convert ETH packet offset 0 to byte 5 contains destination MAC.
	memcpy(data,packet,6);
	//BOOST_LOG_TRIVIAL(debug) << ifname << " Destination MAC ";
	//hexa_print((unsigned char*) data,6);
	//Storing destination MAC
	uint64_t destination_mac = char_to_mac((unsigned char*) data);
	BOOST_LOG_TRIVIAL(debug) << ifname << " Destination MAC " << format_mac_addr((uint8_t*)&destination_mac);
	BOOST_LOG_TRIVIAL(debug) << destination_mac << std::endl;
	BOOST_LOG_TRIVIAL(debug) << ifname << " BUFF SIZE " << buffer.size();
	delete data;
	return destination_mac;
}
uint64_t SwitchSocket::get_source_mac(const char* packet){

	//Allocating space for temporal MAC.
	char *data = (char*) malloc(6*sizeof(char));
	//Copying data to conversion ETH packet offset 6 to byte 11 contains destination MAC.
	memcpy(data,&packet[6],6);
	//BOOST_LOG_TRIVIAL(debug) << ifname << " Source MAC " ;
	//hexa_print((unsigned char*) data,6);
	uint64_t source_mac = char_to_mac((unsigned char*)data);
	BOOST_LOG_TRIVIAL(debug) << ifname << " Source MAC " << format_mac_addr((uint8_t*)&source_mac);
	BOOST_LOG_TRIVIAL(debug) << source_mac << std::endl;
	delete data;
	return source_mac;
}
void SwitchSocket::update_bridge_table(uint64_t source_mac){
	//Inserting inside bridge table.
	BOOST_LOG_TRIVIAL(debug) << "BRIDGE TABLE REF " << bridge_table << "of size " << bridge_table->size();
	(*bridge_table)[source_mac] = this;

	BOOST_LOG_TRIVIAL(debug) << ifname << " BUFF SIZE " << buffer.size();
	BOOST_LOG_TRIVIAL(debug) << ifname << " MAC TABLE: " ;
	//Deciding forwarding.
	for ( mac_table::const_iterator brtable_it = bridge_table->cbegin();
			brtable_it != bridge_table->cend(); brtable_it++){
		BOOST_LOG_TRIVIAL(debug) << "MAC:  " << brtable_it->first << " ON iface: " << brtable_it->second->ifname;
	}
}
void SwitchSocket::forward_current_to_port(SwitchSocket *to){
	BOOST_LOG_TRIVIAL(debug) << ifname << " forwarding packet to " << to->ifname;
	to->l2socket.send(buffer.data());
}
void SwitchSocket::forward_current(uint64_t destination_mac){
	try{
		SwitchSocket* knownhandler = bridge_table->at(destination_mac);
		BOOST_LOG_TRIVIAL(debug) <<  "known MAC-PORT ";
		forward_current_to_port(knownhandler);
	}catch (std::out_of_range &e){
		BOOST_LOG_TRIVIAL(debug) << ifname << " Unknown MAC-PORT " ;
		BOOST_FOREACH(SwitchSocket* element, brothers) {
			if (element != this)forward_current_to_port(element);
		}
	}
}
void SwitchSocket::handle_packet(const boost::system::error_code& error,
	    size_t bytes_transferred){
	// Moving data to my process input sequence.
	buffer.commit(bytes_transferred);
	//Storing temporal data in char*.
    const char* packet = boost::asio::buffer_cast<const char*>(buffer.data());
	BOOST_LOG_TRIVIAL(debug) << ifname <<  " BUFF SIZE " << buffer.size();
	hexa_print((const unsigned  char*) packet,bytes_transferred);
	uint64_t destination_mac = get_destination_mac(packet);
	uint64_t source_mac = get_source_mac(packet);
	//Updating Bridge table.
	update_bridge_table(source_mac);
	// Calling forwarding.
	forward_current(destination_mac);
	// Releasing buffer data.
	buffer.consume(bytes_transferred);
	//Injecting new event handler for the next packet.
	start();
}


