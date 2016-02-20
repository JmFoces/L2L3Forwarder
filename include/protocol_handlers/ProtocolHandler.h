/*
 * L3ProtocolHandler.h
 *
 *  Created on: Jan 22, 2016
 *      Author: xshell
 */

#ifndef PROTOCOLHANDLER_H_
#define PROTOCOLHANDLER_H_
#include <string>
#include <sockets/L2Socket.h>
#include <boost/asio.hpp>
static std::string BASE_PROTO_HANDLER_NAME = "BaseProtoHandler";

/*
 * Stands base structure of a class implementing a Internet-Protocol.
 */
class ProtocolHandler {
public:
	std::string* name;
	uint16_t mux_key;
	ProtocolHandler(){
		mux_key=0xff;
		name = &BASE_PROTO_HANDLER_NAME;
	};
	virtual ~ProtocolHandler(){};

	virtual std::pair<boost::asio::streambuf*,size_t> handle_packet(
		boost::asio::streambuf* recv_data,
		L2Socket* input_sock,
		L2Socket** output_sock,
		uint8_t *dst_mac,
		uint16_t *response_mux_key)=0;
};


#endif /* PROTOCOLHANDLER_H_ */
