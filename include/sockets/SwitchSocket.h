/*
 * SwitchSocket.h
 *
 *  Created on: Jan 21, 2016
 *      Author: xshell
 */

#ifndef SWITCHSOCKET_H_
#define SWITCHSOCKET_H_
#include <sockets/L2Socket.h>


/**
 * Represents a Port of the switch.
 * Its main responsibility is to drive the Ethernet packet through the implementation of the switch algorithm.
 * This algorithm is implemented in handle_packet method.
 */
class SwitchSocket :public L2Socket{
public:
	//Attributes
	static mac_table *bridge_table;
	static list<SwitchSocket*> brothers;
	//Methods.
	/**
	 * SwitchSocket Constructor
	 * Adds this object to shared static brother list.
	 * @param ifname string with name like eth1
	 * @param io_service* pointer to the io_service that will manage the underlying socket.
	 */
	SwitchSocket(string ifname,boost::asio::io_service* io_service);
	/**
	 * L2Socket destructor.
	 * Pending to implement correct release.
	 */
	virtual ~SwitchSocket();
	/*
	 * Calls the brother object to write the current buffer contents to its transmission medium.
	 * @param SwitchSocket* which will forward the packet.
	 */
	void forward_current_to_port(SwitchSocket * to);
	/*
	 * Decides (based on bridge table) whether unicast the packet just to one brother
	 * or to broadcast the packet to all brothers.
	 * @param uint64_t containing destination mac address of the packet inside buffer.
	 */
	void forward_current(uint64_t destination_mac);
	/*
	 * Extracts and converts destination mac from a char* pointer.
	 * Warning, this considers that you pass a pointer to a char array that contains at least 6 elements.
	 * @param char* containing packet contents.
	 * @return uint64_t containing destination mac.
	 */
	uint64_t get_destination_mac(const char* packet);
	/*
	 * Extracts and converts source mac from a char* pointer.
	 * Warning, this considers that you pass a pointer to a char array that contains at least 12 elements.
	 * @param char* containing packet contents.
	 * @return uint64_t containing source mac.
	 */
	uint64_t get_source_mac(const char* packet);
	/*
	 * Updates the bridge table to contain an entry or update the one inside. To this object.
	 * Remember that bridge table maps a  mac -> SwitchSocket*
	 * @param uint64_t containint source mac to insert/update
	 * @return uint64_t containing source mac.
	 */
	void update_bridge_table(uint64_t source_mac);
	/*
	 * Carry out the whole Ethernet frame  switching process.
	 * @param const boost::system::error_code&  containing error code,
	 * @size_t containing the number of bytes that are in the buffer. (THIS VALUE MUST NEVER EXCEED MTU+14 most likely 1514.
	 */
	void handle_packet(
			const boost::system::error_code& error,
			size_t bytes_transferred);

	inline static mac_table* get_bridge_table(){
		return bridge_table;
	}
};


#endif /* SWITCHSOCKET_H_ */
