/*
 * IPProtocolHandler.h
 *
 *  Created on: Jan 26, 2016
 *      Author: xshell
 */

#ifndef PROTOCOL_HANDLERS_IPPROTOCOLHANDLER_H_
#define PROTOCOL_HANDLERS_IPPROTOCOLHANDLER_H_
// /usr/include/netinet/ip.h iphdr defined and ip
#include <map>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <net/ethernet.h>
#include <sockets/L3Socket.h>
#include <netinet/ip.h>
#include <protocol_handlers/ProtocolHandler.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

// ./netinet/if_ether.h ether_arp
class L3Socket; //Declare this so linker can find it.


static std::string IP_PROTO_HANDLER_NAME = "IP";
class IPProtocolHandler : public ProtocolHandler {
public:
	static IPProtocolHandler* getInstance();
	IPProtocolHandler();
	virtual ~IPProtocolHandler();
	void add_ip_addr(in_addr_t ip, in_addr_t netmask,L3Socket* ip_socket);
	unsigned short cksum(struct ip *ip, int len);
	std::pair<boost::asio::streambuf*,size_t> handle_packet(boost::asio::streambuf* buffer,
			L2Socket* input_sock,
			L2Socket** output_sock,
			uint8_t* mac_dst,
			uint16_t *response_mux_key);
	void decode_header(boost::asio::streambuf*,ip*);
	std::pair<boost::asio::streambuf*,size_t> build_routed_packet(
			ip* ip_header,
			boost::asio::streambuf* buffer,
			in_addr_t from_addr);
private:
	static IPProtocolHandler* instance;
};

#endif /* PROTOCOL_HANDLERS_IPPROTOCOLHANDLER_H_ */
