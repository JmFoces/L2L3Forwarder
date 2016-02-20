/*
 * L2Reader.h
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */

#ifndef L2READER_H_
#define L2READER_H_

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
/*
 * This class wraps the logic necessary to asynchronous handle events on
 * L2 sockets.
 */


class L2Socket {
public:
	//Attributes
	u_int8_t mac_addr[6];
	static u_int8_t broadcast_addr[ETH_ALEN];
	string ifname;
	//Methods
	/*
	 * L2SockHanlder Constructor
	 * Sets internal variables.
	 * Wakes up interface given by name in promiscuous mode.
	 * Inserts itself inside the static member list brothers.
	 * @param ifname string with name like eth1
	 * @param io_service *io_service pointing to the io_service that manages the underlying socket.
	 */
	L2Socket(string ifname,boost::asio::io_service* io_service);
	/*
	 * L2Socket destructor
	 */
	virtual ~L2Socket();
	/*
	 * Injects a handler for a received_packet event to be handled by received_packet.
	 */
	void start();

	/*
	 * Interface method for sibling classes to implement the packet handling.
	 */
	virtual void handle_packet(
				const boost::system::error_code& error,
			    size_t bytes_transferred)=0;
	/*
	 * Just build ethernet header and sends  the packet.
	 * Deletes the buffer.
	 */
	std::size_t send(
			uint8_t* dst_mac,
			uint16_t packet_type,
			boost::asio::streambuf* buffer,
			size_t data_size
	);
	/*
	 * Just build ether_header from a packet buffer.
	 */
	ether_header build_header(const char* packet);
	/*
	 * Just check if packet destination l2 address (MAC) is the same as this instance.
	 */
	bool packet_for_me(ether_header eth_hdr);
	/*
	 * Just returns multiplexing key for an Ethernet packet.
	 * Consider that logic is more complex and it just works for ethernet with no tagging.
	 * Otherwise it makes a mistake by not considering that the offset is tampered by the tag
	 * and it will build the multiplexing key from the VLAN tag. Really bad :D.
	 */
	uint16_t get_ethertype(const char* packet);


	raw_protocol_t::socket l2socket;
protected:
	//Attributes
	boost::asio::io_service* io_service;
	sockaddr_ll sockaddr;
	boost::asio::streambuf buffer;
private:
	/*
	 * This method is the handler for a received_packet event.
	 */
	void received_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);
};


#endif /* L2READER_H_ */
