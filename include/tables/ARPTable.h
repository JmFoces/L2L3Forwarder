/*
 * ARPTable1.h
 *
 *  Created on: Jan 31, 2016
 *      Author: xshell
 */

#ifndef TABLES_ARPTABLE_H_
#define TABLES_ARPTABLE_H_

#include <map>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <net/ethernet.h>
#include <sockets/L3Socket.h>
#include <protocol_handlers/ProtocolHandler.h>
#include <protocol_handlers/IPProtocolHandler.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

// ./netinet/if_ether.h ether_arp
class L3Socket; //Declare this otherwise so linker can find it.
typedef std::map<uint64_t,L3Socket*> arp_table;
static arp_table _arp_table;
class ARPTable{
public:
	///Hardcoded to use HW ADDR LEN 6 and L3 ADDR len 4
	static ARPTable* getInstance();
	virtual ~ARPTable();
	void insert(uint8_t* l3_addr, uint8_t* l2_addr,L3Socket* from_sock);
	std::pair<uint64_t,L3Socket*>* query(uint8_t* l3_addr);
private:
	//Attributes
	boost::mutex lock;
	static ARPTable* instance;
	std::map<uint32_t,std::pair<uint64_t,L3Socket*>*> arp_table;
	//Methods
	ARPTable();
};
#endif /* TABLES_ARPTABLE_H_ */
