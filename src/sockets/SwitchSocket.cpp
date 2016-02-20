/*
 * SwitchSocket.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#include <sockets/SwitchSocket.h>
list<SwitchSocket*> SwitchSocket::brothers;
SwitchSocket::SwitchSocket(string ifname,boost::asio::io_service* io_service):
	L2Socket(ifname,io_service) {

	brothers.push_back(this);
}

SwitchSocket::~SwitchSocket() {

}

void SwitchSocket::forward_current(SwitchSocket *to){
	BOOST_LOG_TRIVIAL(debug) << ifname << " forwarding packet to " << to->ifname;
	to->l2socket.send(buffer.data());
}

void SwitchSocket::handle_packet(const boost::system::error_code& error,
	    size_t bytes_transferred){
	// Moving data to my process input sequence.
	buffer.commit(bytes_transferred);
	//Storing temporal data in char*.
    const char* packet = boost::asio::buffer_cast<const char*>(buffer.data());
	BOOST_LOG_TRIVIAL(debug) << ifname <<  " BUFF SIZE " << buffer.size();
    hexa_print((const unsigned  char*) packet,bytes_transferred);
    //Allocating space for temporal MACs.
	char *data = (char*) malloc(6*sizeof(char));
	//Copying data to conversion ETH packet offset 0 to byte 5 contains destination MAC.
	memcpy(data,packet,6);
	BOOST_LOG_TRIVIAL(debug) << ifname << " Destination MAC ";
	hexa_print((unsigned char*) data,6);
	//Storing destination MAC
	uint64_t destination_mac = char_to_mac((unsigned char*) data);
	BOOST_LOG_TRIVIAL(debug) << destination_mac << std::endl;
	BOOST_LOG_TRIVIAL(debug) << ifname << " BUFF SIZE " << buffer.size();
	hexa_print((const unsigned  char*) packet,bytes_transferred);
	//Repeating the same process at packet offset 6 + 6 that contains source mac.
	memcpy(data,&packet[6],6);
	BOOST_LOG_TRIVIAL(debug) << ifname << " Source MAC ";
	hexa_print((unsigned char*) data,6);
	uint64_t source_mac = char_to_mac((unsigned char*)data);
	BOOST_LOG_TRIVIAL(debug) << source_mac << std::endl;
	//Inserting inside bridge table.
	bridge_table[source_mac] = this;
	//Releasing temporal data.
	delete data;

	BOOST_LOG_TRIVIAL(debug) << ifname << " BUFF SIZE " << buffer.size();
	BOOST_LOG_TRIVIAL(debug) << ifname << " MAC TABLE: " ;
	//Deciding forwarding.
	for ( mac_table::const_iterator brtable_it = bridge_table.begin();
			brtable_it != bridge_table.end(); brtable_it++){
		BOOST_LOG_TRIVIAL(debug) << "MAC:  " << brtable_it->first << " ON iface: " << brtable_it->second->ifname;
	}
	// Calling forwarding.

	try{
		SwitchSocket* knownhandler = bridge_table.at(destination_mac);
		BOOST_LOG_TRIVIAL(debug) <<  "known MAC-PORT ";
		forward_current(knownhandler);
	}catch (std::out_of_range &e){
		BOOST_LOG_TRIVIAL(debug) << ifname << " Unknown MAC-PORT " ;
		BOOST_FOREACH(SwitchSocket* element, brothers) {
			forward_current(element);
		}
	}

	/*
	 *const char* packet = boost::asio::buffer_cast<const char*>(buffer.data());
	 *hexa_print(packet,bytes_transferred);
	*/
	// Releasing buffer data.
	buffer.consume(bytes_transferred);
	//Injecting new event handler for the next packet.
	start();
}


