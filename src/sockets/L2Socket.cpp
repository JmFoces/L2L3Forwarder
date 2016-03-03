/*
 * L2Reader.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */

#include <sockets/L2Socket.h>

u_int8_t L2Socket::broadcast_addr[ETH_ALEN] = {0xff,0xff,0xff,0xff,0xff,0xff};

L2Socket::L2Socket(string ifname, boost::asio::io_service* io_service):l2socket(*io_service, raw_protocol_t(PF_PACKET, SOCK_RAW))
{
	// Setting attrs.
	this->io_service = io_service;
	this->ifname = ifname;
	// Getting native pf_packet, l2 sock.
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sll_family = PF_PACKET;
	sockaddr.sll_protocol = htons(ETH_P_ALL);
	sockaddr.sll_ifindex = if_nametoindex(ifname.c_str());
	sockaddr.sll_hatype = 1;
	l2socket.bind(raw_endpoint_t(&sockaddr, sizeof(sockaddr)));
	int native_fd = l2socket.native_handle();

	struct ifreq ifr;
	// Set promisc mode.
	memset (&ifr, 0, sizeof (struct ifreq));
	strcpy(ifr.ifr_name,ifname.c_str());
	ifr.ifr_flags |= IFF_UP;
	ifr.ifr_flags |= IFF_PROMISC;
	if (ioctl (native_fd, SIOCSIFFLAGS, &ifr) == -1)
	{
		perror ("Error: Could not set flag IFF_PROMISC");
		exit (1);
	}
	BOOST_LOG_TRIVIAL(trace) <<
			ifname << " Entering Promiscuous Mode";

	// Getting MAC interface addr
	ioctl(native_fd, SIOCGIFHWADDR, &ifr);
	for( int s = 0; s < ETH_ALEN; s++ )
	{
		mac_addr[s] = (u_int8_t) ifr.ifr_hwaddr.sa_data[s];
	}
	BOOST_LOG_TRIVIAL(debug)
		<< ifname
		<< " MAC Addr : "
		<<	format_mac_addr(mac_addr);
	printf("\n");
}

L2Socket::~L2Socket() {
	// TODO Auto-generated destructor stub
}

void L2Socket::start(){
	//cout << ifname << " putting event"<<endl;
	l2socket.async_receive(
		buffer.prepare(ETH_MTU+ETH_HDR_MAX_LEN),
		boost::bind(
			  &L2Socket::received_packet,
			  this,
			  boost::asio::placeholders::error,
			  boost::asio::placeholders::bytes_transferred
		)
	);
}

std::size_t L2Socket::send(
	uint8_t* dst_mac,
	u_int16_t packet_type,
	boost::asio::streambuf* data_buff,size_t data_size){
	BOOST_LOG_TRIVIAL(debug)
			<< "Creating Ethernet packet"
			<<" from: "
			<< format_mac_addr(this->mac_addr)
			<<" to: " << format_mac_addr(dst_mac);
	const unsigned char* eth_payload = boost::asio::buffer_cast<const unsigned char*>(data_buff->data());
	///Setting header struct.
	ether_header eth_hdr;
	unsigned char packethdr[ARPHDR_LEN]={};
	memcpy(&eth_hdr.ether_dhost,dst_mac,ETH_ALEN);
	memcpy(&eth_hdr.ether_shost,this->mac_addr,ETH_ALEN);
	eth_hdr.ether_type = packet_type;

	//Writting header for all packets to the output buffer.
	memcpy(&packethdr[0],&eth_hdr.ether_dhost,ETH_ALEN);
	memcpy(&packethdr[ETH_ALEN],&eth_hdr.ether_shost,ETH_ALEN);
	char_shift(
		(unsigned char*)&eth_hdr.ether_type,
		sizeof(uint16_t),
		(uint8_t*)&packethdr[ETH_ALEN*2]
	);
	if (data_size <= ETH_MTU){
		size_t send_size = ETH_HDR_MAX_LEN + data_size;
		unsigned char packet[send_size]= {}; //ZEROED array.

	// Looping & sending data chunks of ETH_MTU ( 1500 bytes with 14 byte header )

		memcpy(&packet[0],packethdr,ETH_HDR_MAX_LEN);

		BOOST_LOG_TRIVIAL(debug)
			<< "eth: payload length "
			<< boost::format("%d") % send_size;
		memcpy(&packet[ETH_HDR_MAX_LEN],eth_payload,data_size);
		BOOST_LOG_TRIVIAL(debug)
			<< "Sending Ethernet packet "
			<< "of len " << send_size << " through " << this->ifname << " "
			<< hexa_print(packet,send_size);
		std::size_t bytes_transferred = l2socket.send(boost::asio::buffer(packet,send_size));
		data_buff->consume(bytes_transferred);
		data_size = data_buff->in_avail();

		delete data_buff;
	}
	return data_size;
}

void L2Socket::received_packet(const boost::system::error_code& error,
	    size_t bytes_transferred){
	this->handle_packet(error,bytes_transferred);
}

ether_header L2Socket::build_header(const char* packet){
	ether_header eth_hdr;
	ether_addr dhost = std_char_to_mac((unsigned char*)packet);
	ether_addr shost = std_char_to_mac((unsigned char*)&packet[6]);
	memcpy(eth_hdr.ether_dhost,dhost.ether_addr_octet,6);
	memcpy(eth_hdr.ether_shost,shost.ether_addr_octet,6);
	eth_hdr.ether_type = get_ethertype(packet);
	return eth_hdr;
}

bool L2Socket::packet_for_me(ether_header eth_hdr){
	if(compare_arr(eth_hdr.ether_shost,mac_addr,ETH_ALEN)){
		BOOST_LOG_TRIVIAL(trace) << "packet comes from my interface drop it";
		return false;
	}
	if	(compare_arr(eth_hdr.ether_dhost,mac_addr,ETH_ALEN)){
		BOOST_LOG_TRIVIAL(trace) << "is unicast for me";
		return true;
	}
	if (compare_arr(eth_hdr.ether_dhost,broadcast_addr,ETH_ALEN)){
		BOOST_LOG_TRIVIAL(trace) << "is broadcast for me";
		return true;
	}
	return false;


}

uint16_t L2Socket::get_ethertype(const char* packet){

	uint16_t mux_key = ((uint16_t)packet[12] << 8) |
						  ((uint16_t)packet[13] << 0);
	BOOST_LOG_TRIVIAL(trace) <<
			ifname << " Checking packet mux key : "<< boost::format("0x%02X")%  packet[12];
	return mux_key;
}
