/*
 * misc.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */


#include <ostream>
#include <misc.h>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

std::string hexa_print(const unsigned char* array, int len){
	std::string result;
	std::stringstream stream;
	for (int i = 0; i < len;i++){
		stream << str(boost::format("%02x") % (unsigned short)array[i]);
	}
	result = stream.str();
	return result;
}

std::string hexa_print(const char* array, int len){
	std::string result;
	std::stringstream stream;
	for (int i = 0; i < len;i++){
		stream << str(boost::format("%02x") % (unsigned short)array[i]);
	}
	result = stream.str();
	return result;
}

uint64_t char_to_mac(unsigned char *data){
	uint64_t result = ((uint64_t)data[0] << 40) |
					  ((uint64_t)data[1] << 32) |
					  ((uint64_t)data[2] << 24) |
					  ((uint64_t)data[3] << 16) |
					  ((uint64_t)data[4] << 8)  |
					  ((uint64_t)data[5] << 0);
	return result;
}
uint32_t char_to_ip(unsigned char *data){
	uint32_t result = ((uint32_t)data[0] << 24) |
					  ((uint32_t)data[1] << 16) |
					  ((uint32_t)data[2] << 8)  |
					  ((uint32_t)data[3] << 0);
		return result;
}
uint32_t int8_ptr_to_uint32(uint8_t *data){
	uint32_t result = ((uint32_t)data[3] << 24) |
					  ((uint32_t)data[2] << 16) |
					  ((uint32_t)data[1] << 8)  |
					  ((uint32_t)data[0] << 0);
	return result;
}
uint32_t int8_ptr_to_uint32_shift(uint8_t *data){
	uint32_t result = ((uint32_t)data[0] << 24) |
					  ((uint32_t)data[1] << 16) |
					  ((uint32_t)data[2] << 8)  |
					  ((uint32_t)data[3] << 0);
	return result;
}

ether_addr std_char_to_mac(unsigned char *data){
	ether_addr result;
	for(int i = 0; i < 6 ; i++){
		result.ether_addr_octet[i] = (u_int8_t)data[i];
	}

	return result;
}


void char_shift(unsigned char* array,uint8_t len,uint8_t* result){
	uint64_t shift = 0;
	for(int lendec = len-1;lendec>=0;lendec--){
		result[shift] = array[lendec];
		shift = shift + 1;
	}
}

boost::format format_mac_addr(uint8_t* mac_addr)
{

		return boost::format("%+02x:%+02x:%+02x:%+02x:%+02x:%+02x")
					% (unsigned short)mac_addr[0]
					% (unsigned short)mac_addr[1]
					% (unsigned short)mac_addr[2]
					% (unsigned short)mac_addr[3]
					% (unsigned short)mac_addr[4]
					% (unsigned short)mac_addr[5];
}
boost::format format_ip_addr(uint8_t* ip_addr)
{
	return boost::format("%+d.%+d.%+d.%+d")
	        	% (uint32_t)ip_addr[0]
				% (uint32_t)ip_addr[1]
			    % (uint32_t)ip_addr[2]
			    % (uint32_t)ip_addr[3];
}
boost::format format_ip_addr(char* ip_addr)
{
	return boost::format("%+d.%+d.%+d.%+d")
		        	% (uint32_t)ip_addr[0]
					% (uint32_t)ip_addr[1]
				    % (uint32_t)ip_addr[2]
				    % (uint32_t)ip_addr[3];
}


bool compare_arr(uint8_t* first,uint8_t *second,size_t size){
	for (size_t i = 0; i < size; i++){
		if (first[i]!=second[i])return false;
	}
	return true;
}


void call_ios(boost::asio::io_service* ios){
	try{
		ios->run();
	}catch(std::exception &e){
		BOOST_LOG_TRIVIAL(fatal) << "Exception raised " << e.what();
	}

}
