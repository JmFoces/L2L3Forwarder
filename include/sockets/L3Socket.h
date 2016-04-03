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
	/*
	 * L3Socket Constructor.
	 * Since its a L2Socket with logic to work at Network layer this constructor adds nothing.
	 */
	L3Socket(string ifname,boost::asio::io_service* io_service);
	virtual ~L3Socket();
	/*
	 * Adds a protocol handler by just putting an entry on the working protocols map with the
	 * de-multiplexing key.
	 * @param Protocol handler pointer to the instance of a protocol handler.
	 */
	void add_protocol(ProtocolHandler*);
	/*
	 * Adds necessary protocol handlers for ARP and IP.
	 * Tells IP Protocol handler instance that the given IP is on this interface.
	 * 			WARNING: This method does not check values. Ensure good ones for the moment!!
	 * @param unsigned char* to a memory region containing 4 bytes meaning the ip address.
	 * @param unsigned char* to a memory region containing 4 bytes meaning the netmask.
	 */
	void add_ip_addr(unsigned char*,unsigned char* netmask);

	/*
	 * Implements the interface to handle packets inside Ethernet
	 */
	void handle_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);
	/*
	 * Demultiplex the packet and calls the matching protocol handler.
	 */
	void handle_l3_packet(ether_header eth_hdr,boost::asio::streambuf* packet_buffer);

};

#endif /* L3SOCKET_H_ */
