/*
 * L3Socket.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#include <sockets/L3Socket.h>

//map<in_addr_t,L3Socket*> L3Socket::brothers; //OLD, before routing table
L3Socket::L3Socket(
		string ifname,
		boost::asio::io_service* io_service
		):
		L2Socket(ifname,io_service) {
}

L3Socket::~L3Socket() {
}
/*
void L3Socket::add_ip(std::string ip_str){
	in_addr ia;
	if (!inet_aton(ip_str.c_str(),&ia)){
		BOOST_LOG_TRIVIAL(debug) << "unable to set ip " << ip_str;
	}
	ips.insert(ia.s_addr);
}
*/
void L3Socket::add_protocol(ProtocolHandler* proto_hdl){
	BOOST_LOG_TRIVIAL(debug) << "Added mux_key "<< hex << proto_hdl->mux_key;
	working_protocols[proto_hdl->mux_key] = proto_hdl;
}

void L3Socket::add_ip_addr(unsigned char* ip_str,unsigned char* netmask){
	BOOST_LOG_TRIVIAL(debug) << ifname <<" Start Working ARP";
	ARPProtocolHandler* arp_hdl = ARPProtocolHandler::getInstance();
	add_protocol(arp_hdl);
	in_addr ip;
	inet_aton((const char* )ip_str,&ip);
	IPProtocolHandler* ip_hdl = IPProtocolHandler::getInstance();
	in_addr netmask_addr;
	inet_aton((const char* )netmask,&netmask_addr);
	ip_hdl->add_ip_addr(ip.s_addr,netmask_addr.s_addr,this);
	add_protocol(ip_hdl);
	BOOST_LOG_TRIVIAL(debug) << ifname <<" Start Working IP";
	BOOST_LOG_TRIVIAL(debug) << "Setting IP Addr " << format_ip_addr((uint8_t*)(&ip.s_addr)) << " Iface "<< ifname;
}
void L3Socket::handle_packet(
		const boost::system::error_code& error,
	    size_t bytes_transferred){
	// Moving data to my process input sequence.
	buffer.commit(ETH_HDR_MAX_LEN);
	const char* eth_header = boost::asio::buffer_cast<const char*>(buffer.data());
	ether_header ethhdr = this->build_header(eth_header);
	if(L2Socket::packet_for_me(ethhdr)){
		buffer.consume(ETH_HDR_MAX_LEN);
		handle_l3_packet(ethhdr,&buffer);
	}
	uint16_t unhandled_data = ETH_MTU-buffer.in_avail();
	BOOST_LOG_TRIVIAL(trace) << "Discarded bytes " << unhandled_data;
	//buffer.consume(bytes_transferred);
	buffer.consume(buffer.size());
	//Injecting new event handler for the next packet.
	start();
}

void L3Socket::handle_l3_packet(
		ether_header eth_hdr,
		boost::asio::streambuf* packet_buffer){
	try{
		ProtocolHandler* proto_hdl = working_protocols.at(eth_hdr.ether_type);
		BOOST_LOG_TRIVIAL(debug) <<  "Found Protocol HDL for " << boost::format("0x%02X") % (unsigned short )eth_hdr.ether_type << " Called " << *proto_hdl->name;
		L3Socket* output_sock=NULL;
		uint8_t mac_dst[ETH_ALEN];
		uint16_t response_mux_key=0;
		std::pair<boost::asio::streambuf*,size_t> response = proto_hdl->handle_packet(
			&buffer,
			this,
			(L2Socket**)&output_sock,
			mac_dst,
			&response_mux_key
		);
		if (response_mux_key==0) response_mux_key = proto_hdl->mux_key;
		if(response.first != NULL){
			BOOST_LOG_TRIVIAL(debug) << "Writting response";
			output_sock->send(mac_dst,response_mux_key,response.first,response.second);
		}

	}catch (std::out_of_range &e){
		BOOST_LOG_TRIVIAL(trace) << ifname << " Not working protocol " << boost::format("0x%02X") % (unsigned short )eth_hdr.ether_type;

	}
}
