/*
 * L3Socket.h
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#ifndef L3SOCKET_H_
#define L3SOCKET_H_
#include <boost/format.hpp>
#include <sockets/L2Socket.h>
#include <protocol_handlers/ARPProtocolHandler.h>
#include <protocol_handlers/IPProtocolHandler.h>
#include <protocol_handlers/ProtocolHandler.h>
#include <tables/RoutingTable.h>
///usr/include/net/if_arp.h struct defines.
// include/linux/if_arp.h
//net/ipv4/arp.c kmode util arp methods

class RoutingTable;
/**
 * Represents a Network Layer socket.
 * Its main responsibility is to handle packets coming from lower layer sockets, L2Socket.
 */

class L3Socket :public L2Socket{
public:
	//Attributes
	map<uint16_t, ProtocolHandler*> working_protocols;
	//Methods
	L3Socket(string ifname,boost::asio::io_service* io_service);
	virtual ~L3Socket();
	void add_protocol(ProtocolHandler*);
	void add_ip_addr(in_addr);
	void add_ip_addr(unsigned char*,unsigned char* netmask);
	//void add_ip(std::string ip_str);
	void handle_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);
	/*
	 * Demultiplex the packet and calls the matching protocol handler.
	 */
	void handle_l3_packet(ether_header eth_hdr,boost::asio::streambuf* packet_buffer);

};

#endif /* L3SOCKET_H_ */
