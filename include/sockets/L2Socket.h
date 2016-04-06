/*
 * L2Reader.h
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */

#ifndef L2SOCKET_H_
#define L2SOCKET_H_

#include <misc.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <boost/foreach.hpp>
#include <stdint.h>
#include <list>
#include <stdio.h>
#include <string.h>
#include <boost/asio/detail/push_options.hpp>
#include <boost/log/trivial.hpp>

using namespace std;

/**
 * Represents a Link Layer socket.
 * Its main responsibility is to receive and write packets from and to the interface, respectively.
 * Offers an interface so sibling classes of upper protocols can receive and write packets to the interface.
 * It decides where to dispatch the packet, most likely the upper layer socket.
 */

class L2Socket {
public:
	//Attributes
	u_int8_t mac_addr[6];
	static u_int8_t broadcast_addr[ETH_ALEN];
	string ifname;
	raw_protocol_t::socket l2socket;
	//Methods
	/**
	 * L2Socket Constructor
	 * Wakes up interface given by name in promiscuous mode.
	 * @param ifname string with name like eth1
	 * @param io_service* pointer to the io_service that will manage the underlying socket.
	 */
	L2Socket(string ifname,boost::asio::io_service* io_service);
	/**
	 * L2Socket destructor
	 * Pending to implement correct release, disable promiscuous mode etc...
	 */
	virtual ~L2Socket();
	/**
	 * Injects a handler for a received_packet event to be handled by received_packet.
	 */
	void start();

	/**
	 * Interface method for sibling classes to implement the packet handling.
	 */
	virtual void handle_packet(
				const boost::system::error_code& error,
			    size_t bytes_transferred)=0;

	/**
	 * Just build Ethernet header and sends the packet.
	 * Deletes the buffer.
	 * @param uint8_t* where destination MAC address is stored.
	 * @param packet_type is the upper protocol multiplexing key.
	 * @param boost:asio::streambuf* containing the data to be written as payload.
	 * @param size_t indicating the length of data. (I know that maybe it needs a fix so the streambuf may provide this info).
	 */
	std::size_t send(
			uint8_t* dst_mac,
			uint16_t packet_type,
			boost::asio::streambuf* buffer,
			size_t data_size
	);

	/**
	 * Just build ether_header from a packet buffer.
	 * It just works with Ethernet, without VLAN tagging.
	 * @param const char*  packet data.
	 */
	ether_header build_header(const char* packet);

	/**
	 * Decides whether the packet should be handled or not basing on destination MAC address.
	 * @param ethernet_header struct containing necessary data.
	 */
	bool packet_for_me(ether_header eth_hdr);

	/**
	 * Just returns multiplexing key for an Ethernet packet.
	 * Consider that logic is more complex and it just works for ethernet with no tagging.
	 * Otherwise it makes a mistake by not considering that the offset is tampered by the tag
	 * and it will build the multiplexing key from the VLAN tag. Really bad :D.
	 * @const char* packet data.
	 */
	uint16_t get_ethertype(const char* packet);


protected:
	//Attributes
	boost::asio::io_service* io_service;
	sockaddr_ll sockaddr;
	boost::asio::streambuf buffer;

private:
	/**
	 * This method is the handler for a received_packet event.
	 * @param boost::system::error_code operation status.
	 * @param size_t bytes_transferred up to n bytes read from the interface.
	 */
	void received_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);
};


#endif /* L2SOCKET_H_ */
