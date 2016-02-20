/*
 * IPProtocolHandler.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: xshell
 */

#include <protocol_handlers/IPProtocolHandler.h>



IPProtocolHandler* IPProtocolHandler::instance = NULL;

IPProtocolHandler* IPProtocolHandler::getInstance(){
	if(IPProtocolHandler::instance == NULL){
		IPProtocolHandler::instance = new IPProtocolHandler();
	}
	return instance;
}

IPProtocolHandler::IPProtocolHandler() {
	// TODO Auto-generated constructor stub
	BOOST_LOG_TRIVIAL(debug) << "Created IP HDL";
	mux_key= ETHERTYPE_IP;
	name = &IP_PROTO_HANDLER_NAME;
}

IPProtocolHandler::~IPProtocolHandler() {
	// TODO Auto-generated destructor stub
}

void IPProtocolHandler::add_ip_addr(in_addr_t ip, in_addr_t netmask,L3Socket* ip_socket){
	RoutingTable::getInstance()->insert(
		netmask,
		ip,
		ip_socket,
		(in_addr_t)0
	);
}

unsigned short IPProtocolHandler::cksum(ip* ip_header,int len){
    long sum = 0;  /* assume 32 bit long, 16 bit short */
    unsigned short* ptr = (unsigned short*)ip_header;
    BOOST_LOG_TRIVIAL(trace) << "Calculus of checksum ";
    BOOST_LOG_TRIVIAL(trace) << hexa_print((const unsigned char* ) ip_header,len);
    while(len > 1){
    	BOOST_LOG_TRIVIAL(trace)
    			<< "sum = " << boost::format("0x%+04X") % sum
    			<< " + " << boost::format("0x%+04X") % *ptr;
		sum += *ptr++;
		if(sum & 0x80000000){  /* if high order bit set, fold */
			sum = (sum & 0xFFFF) + (sum >> 16);
			BOOST_LOG_TRIVIAL(debug)
					<< "&&0x80000000 sum = " << boost::format("0x%+04X") % sum;
		}
		len -= 2;
    }

    if(len){       /* take care of left over byte */
      sum += (unsigned short) *(unsigned char *)ptr;
      BOOST_LOG_TRIVIAL(trace)
      		<< "left over byte sum = " << boost::format("0x%+04X") % sum
      		<< " + " << boost::format("0x%+04X") % *ptr;
    }
    while(sum>>16){
    	sum = (sum & 0xFFFF) + (sum >> 16);
    	BOOST_LOG_TRIVIAL(trace)
    			<< "shift sum = " << boost::format("0x%+04X") % sum;
    }
    return ~sum;
  }
std::pair<boost::asio::streambuf*,size_t> IPProtocolHandler::handle_packet(
		boost::asio::streambuf* buffer,
		L2Socket* input_sock,
		L2Socket** output_sock,
		uint8_t* mac_dst,
		uint16_t *response_mux_key){
	BOOST_LOG_TRIVIAL(trace) << "Handling ip packet of len "<< buffer->in_avail();
	ip ip_hdr;
	decode_header(buffer,&ip_hdr);

	//Just route the packet here.
	std::pair<L3Socket*,uint64_t> enroute_to = RoutingTable::getInstance()->query(
		ip_hdr.ip_dst.s_addr
	);
	if(enroute_to.first != NULL){
		*output_sock = enroute_to.first;
		uint32_t next_hop = ((uint32_t*)&enroute_to.second)[0];
		uint32_t if_addr  = ((uint32_t*)&enroute_to.second)[1];

		if(next_hop == 0x00000000){
			// i am on the same link than the remote host

			std::pair<uint64_t,L3Socket*>* arp_entry = ARPTable::getInstance()->query(
				(uint8_t*) &ip_hdr.ip_dst.s_addr
			);
			if(arp_entry != NULL){
				//memcpy(mac_dst,&((uint8_t*) &arp_entry->first)[2],ETH_ALEN);
				char_shift(&((uint8_t*) &arp_entry->first)[0],ETH_ALEN,mac_dst);
				BOOST_LOG_TRIVIAL(debug) << "Routing packet to "
					<<" Interface "<< (*output_sock)->ifname
					<<" Address "  << format_ip_addr((uint8_t*)&ip_hdr.ip_dst.s_addr)
					<<" With mac " << format_mac_addr(mac_dst);
				return build_routed_packet(&ip_hdr,buffer,if_addr);
			}else{
				BOOST_LOG_TRIVIAL(trace) << "Cannot get ARP entry Queriying";
				memset(mac_dst,0xFF,ETH_ALEN);
				*response_mux_key =  ETHERTYPE_ARP;
				BOOST_LOG_TRIVIAL(trace) << "Asking ARP "
					<<" Interface "<< (*output_sock)->ifname
					<<" Address "  << format_ip_addr((uint8_t*)&ip_hdr.ip_dst.s_addr)
					<<" From If addr " << format_mac_addr((uint8_t*)&if_addr);
				return ARPProtocolHandler::getInstance()->create_request(
					*output_sock,
					if_addr,
					ip_hdr.ip_dst.s_addr
				);
			}
		}


	}else{
		BOOST_LOG_TRIVIAL(trace) << "Dunno where to route the packet";
	}
	return std::pair<boost::asio::streambuf*,size_t>(NULL,0);
}
void IPProtocolHandler::decode_header(
		boost::asio::streambuf* buffer,ip* ip_header
){
	BOOST_LOG_TRIVIAL(trace) << "decoding ip header ";
	buffer->commit(1);
	const unsigned char* ip_v_hdr_len = boost::asio::buffer_cast<const unsigned char*>(buffer->data());
	BOOST_LOG_TRIVIAL(debug)
					<< "ip: HDR LEN & Ver "
					<< boost::format("0x%+02x") % (unsigned int)ip_v_hdr_len[0];
	unsigned int hdr_len=0x00000000;
	char_shift((unsigned char*)&ip_v_hdr_len[0],1,(uint8_t*)&hdr_len);
	BOOST_LOG_TRIVIAL(debug)
		<< "ip: HDR LEN & Ver "
		<< boost::format("0x%+02x") % hdr_len;
	ip_header->ip_hl = hdr_len;
	//Somehow bit fields take just the least important 4 bit. this is confusing:-S.
	unsigned int header_length =  hdr_len & (unsigned int) 0x0000000F;
	uint8_t ip_version = hdr_len & (unsigned int)  0xFFFFFFF0;
	ip_header->ip_v = (uint8_t)ip_version >> 4 ; // Set most important to least
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: stored header length "
		<< boost::format("0x%+02x") % (unsigned int)ip_header->ip_hl
		<< "\n ip: stored ver "
		<< boost::format("0x%+02x") % (unsigned int)ip_header->ip_v;
	BOOST_LOG_TRIVIAL(trace)
			<< "ip: ver "
			<< boost::format("0x%+02x") % (unsigned int) ip_version;
	BOOST_LOG_TRIVIAL(debug)
			<< "ip: header length  "
			<< boost::format("0x%+02x") %(unsigned int) header_length;
	header_length= header_length*4;
	buffer->consume(1);
	buffer->commit(header_length-1); //Care we have polled one byte already.
	BOOST_LOG_TRIVIAL(trace)
		<< "Decode buffer commited (consider that first byte has been exploded yet)"
		<< boost::format("0x%+d") % header_length;


	// Getting format of IP version+hdr len
	const unsigned char* ip_raw_hdr = boost::asio::buffer_cast<const unsigned char*>(buffer->data());
	// Getting Type of service
	char_shift((unsigned char*)&ip_raw_hdr[0],1,(uint8_t*)&ip_header->ip_tos);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: Type of service "
				<< boost::format("0x%+02x") % ip_header->ip_tos;
	// IP Total len
	char_shift((unsigned char*)&ip_raw_hdr[1],2,(uint8_t*)&ip_header->ip_len);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: IP Total len "
				<< std::to_string(ip_header->ip_len) <<  " "
				<< boost::format("0x%+02x") % ip_header->ip_len;
	// IP identifier
	char_shift((unsigned char*)&ip_raw_hdr[3],2,(uint8_t*)&ip_header->ip_id);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: IP identifier "
				<< boost::format("0x%+02x") % ip_header->ip_id;
	 // IP fragment offset
	char_shift((unsigned char*)&ip_raw_hdr[5],2,(uint8_t*)&ip_header->ip_off);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip:  IP fragment offset "
				<< boost::format("0x%+02x") % ip_header->ip_off;
	// IP ttl
	char_shift((unsigned char*)&ip_raw_hdr[7],1,(uint8_t*)&ip_header->ip_ttl);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: ttl "
				<< boost::format("0x%+02x") % (short int)ip_header->ip_ttl;
	// IP higher protoid
	char_shift((unsigned char*)&ip_raw_hdr[8],1,(uint8_t*)&ip_header->ip_p);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: higher protoid "
				<< boost::format("0x%+02x") % (short int) ip_header->ip_p;
	// IP Checksum
	char_shift((unsigned char*)&ip_raw_hdr[9],2,(uint8_t*)&ip_header->ip_sum);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: Checksum "
				<< boost::format("0x%+02x") % ip_header->ip_sum;
	// IP S addr
	memcpy(&ip_header->ip_src.s_addr,&ip_raw_hdr[11],4);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: S addr "
				<< format_ip_addr((uint8_t*)&ip_header->ip_src.s_addr);
	// IP D addr
	memcpy(&ip_header->ip_dst.s_addr,&ip_raw_hdr[15],4);
	BOOST_LOG_TRIVIAL(trace)
				<< "ip: D addr "
				<< format_ip_addr((uint8_t*)&ip_header->ip_dst.s_addr);
	//BOOST_LOG_TRIVIAL(debug) << "ip: hrd (format of HWaddress) " << boost::format("0x%+02x") % arp_hdr->ar_hrd;

	buffer->consume(header_length-1);
	BOOST_LOG_TRIVIAL(trace)
			<< "Decode buffer consumed "
			<< boost::format("%+d") % header_length;
	BOOST_LOG_TRIVIAL(trace) << "Handling ip packet pending len "<< buffer->in_avail();
}

std::pair<boost::asio::streambuf*,size_t> IPProtocolHandler::build_routed_packet(
		ip* ip_header,
		boost::asio::streambuf* buffer,
		in_addr_t from_addr){
	//This case we just decrease by one the IP TTL so checksum will get decreased 1.
	//Every change implies a change on the sum.
	ip_header->ip_ttl = ip_header->ip_ttl - 1;
	ip_header->ip_sum = 0;

	uint16_t header_length = (ip_header->ip_hl*4);
	unsigned char ip_raw_hdr[header_length] = {};

	ip_raw_hdr[0]= (unsigned char) (
		(uint8_t) ip_header->ip_v << 4|(uint8_t) ip_header->ip_hl << 0
	);
	BOOST_LOG_TRIVIAL(trace)
		<< "Wrote IP_V_HDRL "
		<< boost::format("head len 0x%+02x ipversion 0x%+02x")
			% (uint16_t)ip_header->ip_hl
			% (uint16_t)ip_header->ip_v << " pkt "
		<< boost::format("0x%+02x") % (int) ip_raw_hdr[0];

	char_shift((unsigned char*)&ip_header->ip_tos,1,(uint8_t*)&ip_raw_hdr[1]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote Type of service "
		<< boost::format("0x%+02x") % ip_header->ip_tos << " pkt "
		<< boost::format("0x%+02x") % (int) ip_raw_hdr[1];

	char_shift((unsigned char*)&ip_header->ip_len,2,(uint8_t*)&ip_raw_hdr[2]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote IP Total len "
		<< boost::format("0x%+02x") % ip_header->ip_len << " pkt "
		<< boost::format("0x%+02x%+02x") % (u_short)ip_raw_hdr[2] % (u_short)ip_raw_hdr[3];

	char_shift((unsigned char*)&ip_header->ip_id,2,(uint8_t*)&ip_raw_hdr[4]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote IP identifier "
		<< boost::format("0x%+04x") % ip_header->ip_id << " pkt "
		<< boost::format("0x%+02x%+02x") % (int)ip_raw_hdr[4] % (int)ip_raw_hdr[5];

	char_shift((unsigned char*)&ip_header->ip_off,2,(uint8_t*)&ip_raw_hdr[6]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip:  Wrote IP fragment offset "
		<< boost::format("0x%+04x") % ip_header->ip_off << " pkt "
		<< boost::format("0x%+02x%+02x") % (int)ip_raw_hdr[6]% (int)ip_raw_hdr[7];

	char_shift((unsigned char*)&ip_header->ip_ttl,1,(uint8_t*)&ip_raw_hdr[8]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote ttl "
		<< boost::format("0x%+02x") % (int)ip_header->ip_ttl << " pkt "
		<< boost::format("0x%+02x") % (int)ip_raw_hdr[8];

	char_shift((unsigned char*)&ip_header->ip_p,1,(uint8_t*)&ip_raw_hdr[9]);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote higher protoid "
		<< boost::format("0x%+02x") % (int)ip_header->ip_p<< " pkt "
		<< boost::format("0x%+02x") % (int)ip_raw_hdr[9];

	char_shift((unsigned char*)&ip_header->ip_sum,2,(uint8_t*)&ip_raw_hdr[10]);


	memcpy(&ip_raw_hdr[12],(uint8_t*)&ip_header->ip_src.s_addr,4);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote S addr "
		<< boost::format("0x%+04x") % ip_header->ip_src.s_addr<< " pkt "
		<< format_ip_addr(&ip_raw_hdr[12]);

	memcpy(&ip_raw_hdr[16],(uint8_t*)&ip_header->ip_dst.s_addr,4);
	BOOST_LOG_TRIVIAL(trace)
		<< "ip: Wrote D addr "
		<< boost::format("0x%+04x") % ip_header->ip_dst.s_addr<< " pkt "
		<< format_ip_addr(&ip_raw_hdr[16]);

	boost::asio::streambuf* result = new boost::asio::streambuf();
	streamsize header_size = header_length;
	streamsize payload_size = ip_header->ip_len-header_length;
	streamsize packet_len = ip_header->ip_len;
	ip_header->ip_sum = cksum((ip*)ip_raw_hdr,header_length);
	memcpy(&ip_raw_hdr[10],(uint8_t*)&ip_header->ip_sum,2);
	BOOST_LOG_TRIVIAL(trace)
			<< "ip: Wrote Checksum "
			<< boost::format("0x%+02x") % ip_header->ip_sum<< " pkt "
			<< boost::format("0x%+02x%+02x") % (u_short)ip_raw_hdr[10] % (u_short)ip_raw_hdr[11];
	result->sputn((char*)ip_raw_hdr,header_size);
	const unsigned char* ip_payload = boost::asio::buffer_cast<const unsigned char*>(buffer->data());
	result->sputn((const char*)ip_payload,payload_size);
	result->commit(packet_len);
	return std::pair<boost::asio::streambuf*,size_t>(result,packet_len);
}

