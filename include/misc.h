/*
 * misc.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */



#ifndef _MISC
#define _MISC

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <set>
#include <map>
#include <string>
#include <net/ethernet.h>
#include <net/if_arp.h>
#include <netinet/if_ether.h>

typedef boost::asio::generic::raw_protocol raw_protocol_t;
typedef boost::asio::generic::basic_endpoint<raw_protocol_t> raw_endpoint_t;

static const uint16_t ETH_MTU = ETHERMTU;
static const uint8_t ETH_HDR_MAX_LEN = ETH_HLEN; //Without considering VLAN Tagging.
static const uint8_t ARPHDR_LEN = sizeof(arphdr);
static const uint8_t ARPPAY_LEN = sizeof(ether_arp)-ARPHDR_LEN; // should be 20

std::string hexa_print(const unsigned char* array, int lenght);
std::string hexa_print(const char* array, int lenght);
uint64_t char_to_mac(unsigned char *data);
uint32_t char_to_ip(unsigned char *data);
ether_addr std_char_to_mac(unsigned char *data);
uint32_t int8_ptr_to_uint32(uint8_t *data);
uint32_t int8_ptr_to_uint32_shift(uint8_t *data);
void char_shift(unsigned char* array,uint8_t len,uint8_t* result);
boost::format format_mac_addr(uint8_t* mac_addr);
boost::format format_ip_addr(uint8_t* ip_addr);
boost::format format_ip_addr(char* ip_addr);
bool compare_arr(uint8_t* first,uint8_t *second,size_t size);
void call_ios(boost::asio::io_service* ios);

#endif
