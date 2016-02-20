/*
 * RoutingTable.h
 *
 *  Created on: Jan 31, 2016
 *      Author: xshell
 */

#ifndef TABLES_ROUTINGTABLE_H_
#define TABLES_ROUTINGTABLE_H_
#include <map>
#include <boost/foreach.hpp>
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


class RoutingTable{
public:
	//       MASK:NET           OUTSOCKET,NEXTHOP_IF_IP           invert order
	std::map<uint64_t,std::pair<L3Socket*,uint64_t>,bool(*)(uint64_t,uint64_t)> routingtable;
	static RoutingTable* getInstance();
	virtual ~RoutingTable();
	static bool key_compare(uint64_t first, uint64_t second );
	void insert(in_addr_t mask, in_addr_t if_addr, L3Socket* output_sock, in_addr_t nexthop);
	std::pair<L3Socket*,uint64_t>  query(in_addr_t ip);
private:
	//Attributes
	boost::mutex lock;
	static RoutingTable* instance;

	//Methods
	RoutingTable();
};

#endif /* TABLES_ROUTINGTABLE_H_ */
