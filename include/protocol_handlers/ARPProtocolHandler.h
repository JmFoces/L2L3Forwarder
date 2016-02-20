/*
 * ARPProtocolHandler.h
 *
 *  Created on: Jan 23, 2016
 *      Author: xshell
 */

#ifndef PROTOCOL_HANDLERS_ARPPROTOCOLHANDLER_H_
#define PROTOCOL_HANDLERS_ARPPROTOCOLHANDLER_H_
#include <map>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <net/ethernet.h>
#include <sockets/L3Socket.h>
#include <protocol_handlers/ProtocolHandler.h>
#include <protocol_handlers/IPProtocolHandler.h>
#include <tables/ARPTable.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

class ARPTable;
static std::string ARP_PROTO_HANDLER_NAME = "ARP";
class ARPProtocolHandler : public ProtocolHandler {
public:

	static ARPProtocolHandler* getInstance();
	virtual ~ARPProtocolHandler();
	std::pair<boost::asio::streambuf*,size_t> handle_packet(
		boost::asio::streambuf*,
		L2Socket* input_sock,
		L2Socket** output_sock,
		uint8_t *dst_mac,
		uint16_t *response_mux_key);
	void decode_header(boost::asio::streambuf*,arphdr* arp_hdr);
	void decode_payload(boost::asio::streambuf*,ether_arp* arp_packet);
	boost::asio::streambuf* build_response(
			L2Socket* l2_socket,
			uint8_t* mac_dst,
			ether_arp *arp_packet);
	std::pair<boost::asio::streambuf*,size_t> create_request(L2Socket* l2_socket,in_addr_t from,in_addr_t query);
private:
	static ARPProtocolHandler* instance ;
	ARPTable* arp_table;
	ARPProtocolHandler();
};


#endif /* PROTOCOL_HANDLERS_ARPPROTOCOLHANDLER_H_ */
