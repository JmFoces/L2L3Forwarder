/*
 * SwitchSocket.h
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#ifndef SWITCHSOCKET_H_
#define SWITCHSOCKET_H_
#include <sockets/L2Socket.h>


class SwitchSocket :public L2Socket{
public:
	static mac_table *bridge_table;
	static list<SwitchSocket*> brothers;
	SwitchSocket(string ifname,boost::asio::io_service* io_service);
	virtual ~SwitchSocket();
	void forward_current(SwitchSocket * to);
	void received_packet(
		const boost::system::error_code& error,
		size_t bytes_transferred);
	void handle_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);
	inline static mac_table* get_bridge_table(){
		return bridge_table;
	}
};


#endif /* SWITCHSOCKET_H_ */
