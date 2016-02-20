/*
 * ARPProtocolHandler.cpp
 *
 *  Created on: Jan 23, 2016
 *      Author: xshell
 */

#include "protocol_handlers/ARPProtocolHandler.h"

ARPProtocolHandler* ARPProtocolHandler::instance = NULL;

ARPProtocolHandler::ARPProtocolHandler() {
	// TODO Auto-generated constructor stub
	BOOST_LOG_TRIVIAL(debug) << "Created ARP HDL";
	mux_key= ETHERTYPE_ARP;
	name = &ARP_PROTO_HANDLER_NAME;
	arp_table = ARPTable::getInstance();
}

ARPProtocolHandler::~ARPProtocolHandler() {
	// TODO Auto-generated destructor stub
}


ARPProtocolHandler* ARPProtocolHandler::getInstance(){
	if(ARPProtocolHandler::instance == NULL){
		ARPProtocolHandler::instance = new ARPProtocolHandler();
	}
	return instance;
}


std::pair<boost::asio::streambuf*,size_t> ARPProtocolHandler::handle_packet(
	boost::asio::streambuf* buffer,
	L2Socket* input_sock,
	L2Socket** output_sock,
	uint8_t* mac_dst,
	uint16_t *response_mux_key){
	ether_arp arp_packet;
	decode_header(buffer,&arp_packet.ea_hdr);
	decode_payload(buffer,&arp_packet);
	*output_sock=input_sock;
	if(arp_packet.ea_hdr.ar_op == ARPOP_REQUEST){
		BOOST_LOG_TRIVIAL(debug) << "ARP_Request";

        ARPTable::getInstance()->insert(arp_packet.arp_spa,arp_packet.arp_sha,(L3Socket*)input_sock);
        //ARPTable::getInstance()->query(arp_packet.arp_spa);
        boost::asio::streambuf* result = build_response(*output_sock,mac_dst,&arp_packet);
        return std::pair<boost::asio::streambuf*,size_t>(result,sizeof(arp_packet));

	}else if (arp_packet.ea_hdr.ar_op == ARPOP_REPLY) {
		BOOST_LOG_TRIVIAL(debug) << "ARP_Reply";
		ARPTable::getInstance()->insert(arp_packet.arp_spa,arp_packet.arp_sha,(L3Socket*)input_sock);
		//boost::asio::streambuf* result = build_response(*output_sock,mac_dst,&arp_packet);
		return std::pair<boost::asio::streambuf*,size_t>(NULL,0);
	}
	return std::pair<boost::asio::streambuf*,size_t>(NULL,0);
}
void ARPProtocolHandler::decode_header(boost::asio::streambuf* buffer,arphdr* arp_hdr){
	buffer->commit(ARPHDR_LEN);
	const unsigned char* arp_raw_hdr = boost::asio::buffer_cast<const unsigned char*>(buffer->data());
	BOOST_LOG_TRIVIAL(trace) << "decoding arp header ";
	//hexa_print((const unsigned char*)arp_raw_hdr,ARPHDR_LEN);
	//FIXME Consider the use of std::stoir
	// Getting format of HWaddress
	char_shift((unsigned char*) &arp_raw_hdr[0],2,(uint8_t*)&arp_hdr->ar_hrd);
	// Getting L3 protocol id
	char_shift((unsigned char*)&arp_raw_hdr[2],2,(uint8_t*)&arp_hdr->ar_pro);
	// Getting HW addr len (Ether len), 6
	char_shift((unsigned char*)&arp_raw_hdr[4],1,(uint8_t*)&arp_hdr->ar_hln);
	// Getting L3 proto len addr
	char_shift((unsigned char*)&arp_raw_hdr[5],1,(uint8_t*)&arp_hdr->ar_pln);
	 // Getting ARP OP Code
	char_shift((unsigned char*)&arp_raw_hdr[6],2,(uint8_t*)&arp_hdr->ar_op);
	BOOST_LOG_TRIVIAL(trace) << "arp: hrd (format of HWaddress) " << boost::format("0x%+02x") % arp_hdr->ar_hrd;
	BOOST_LOG_TRIVIAL(trace) << "arp: hln (HW addr len (Ether len) " << boost::format("0x%+02x") % (unsigned short)arp_hdr->ar_hln;
	BOOST_LOG_TRIVIAL(trace) << "arp: pro (L3 protocol addresses id) " << boost::format("0x%+02x") % arp_hdr->ar_pro;
	BOOST_LOG_TRIVIAL(trace) << "arp: pln (L3 proto len addr) " << boost::format("0x%+02x") % (unsigned short)arp_hdr->ar_pln;
	BOOST_LOG_TRIVIAL(trace) << "arp: op (ARP OP code) " << boost::format("0x%+02x") % arp_hdr->ar_op;
	buffer->consume(ARPHDR_LEN);
}
void ARPProtocolHandler::decode_payload(
		boost::asio::streambuf* buffer,
		ether_arp* arp_packet){
	buffer->commit(ARPPAY_LEN); //2*sizeof(Ipaddr) 2*sizeof(Ethaddr)
	const unsigned char* arp_raw_payload = boost::asio::buffer_cast<const unsigned char*>(buffer->data());
	memcpy(&arp_packet->arp_sha,&arp_raw_payload[0],6);
	BOOST_LOG_TRIVIAL(trace) << "Sender HW addr " << format_mac_addr(arp_packet->arp_sha);
	memcpy(&arp_packet->arp_spa,&arp_raw_payload[6],4);
	BOOST_LOG_TRIVIAL(trace) << "Sender L3 addr " << format_ip_addr(arp_packet->arp_spa);
	memcpy(&arp_packet->arp_tha,&arp_raw_payload[10],6);
	BOOST_LOG_TRIVIAL(trace) << "Target HW addr " << format_mac_addr(arp_packet->arp_tha);
	memcpy(&arp_packet->arp_tpa,&arp_raw_payload[16],4);
	BOOST_LOG_TRIVIAL(trace) << "Target L3 addr " << format_ip_addr(arp_packet->arp_tpa);

	buffer->consume(ARPPAY_LEN);
}
boost::asio::streambuf* ARPProtocolHandler::build_response(
		L2Socket* l2_socket,
		uint8_t* mac_dst,
		ether_arp *arp_packet){
	boost::asio::streambuf* response = new boost::asio::streambuf();
	BOOST_LOG_TRIVIAL(debug)
		<< "Writting arp response"
		<<" to: "
		<< format_mac_addr(l2_socket->mac_addr);
	//Setting destination mac
	memcpy(mac_dst,arp_packet->arp_sha,ETH_ALEN);
	arp_packet->ea_hdr.ar_op = ARPOP_REPLY;
	memcpy(arp_packet->arp_tha,l2_socket->mac_addr,ETH_ALEN);
	//Swapping sender & target hw & protocol addresses (pointers)
	char packet[ARPHDR_LEN+ARPPAY_LEN];

	BOOST_LOG_TRIVIAL(trace)
		<< "arp: hrd (format of HWaddress) "
		<< boost::format("0x%+02x") % arp_packet->ea_hdr.ar_hrd;
	char_shift((unsigned char*)&arp_packet->ea_hdr.ar_hrd,sizeof(unsigned short int),(uint8_t*)&packet[0]);

	BOOST_LOG_TRIVIAL(trace)
		<< "arp: pro (L3 protocol addresses id) "
		<< boost::format("0x%+02x") % arp_packet->ea_hdr.ar_pro;
	char_shift((unsigned char*)&arp_packet->ea_hdr.ar_pro,sizeof(unsigned short int),(uint8_t*)&packet[2]);

	BOOST_LOG_TRIVIAL(trace)
		<< "arp: hln (HW addr len (Ether len) "
		<< boost::format("0x%+02x") % (unsigned short)arp_packet->ea_hdr.ar_hln;
	memcpy(&packet[4],&arp_packet->ea_hdr.ar_hln,sizeof(unsigned char ));

	BOOST_LOG_TRIVIAL(trace)
		<< "arp: pln (L3 proto len addr) "
		<< boost::format("0x%+02x") % (unsigned short)arp_packet->ea_hdr.ar_pln;
	memcpy(&packet[5],&arp_packet->ea_hdr.ar_pln,sizeof(unsigned char));

	BOOST_LOG_TRIVIAL(trace)
		<< "arp: op (ARP OP code) "
		<< boost::format("0x%+02x") % arp_packet->ea_hdr.ar_op;
	//memcpy(&packet[6],&arp_packet->ea_hdr.ar_pro,sizeof(unsigned short int));
	char_shift((unsigned char*)&arp_packet->ea_hdr.ar_op,sizeof(unsigned short int),(uint8_t*)&packet[6]);

	// Consider swapping of sender & target addrs ;)
	BOOST_LOG_TRIVIAL(trace) << "Sender HW addr " << format_mac_addr(arp_packet->arp_tha);
	memcpy(&packet[8],arp_packet->arp_tha,ETH_ALEN);
	BOOST_LOG_TRIVIAL(trace) << "Sender L3 addr " << format_ip_addr(arp_packet->arp_tpa);
	memcpy(&packet[14],arp_packet->arp_tpa,sizeof(in_addr_t));
	BOOST_LOG_TRIVIAL(trace) << "Target HW addr " << format_mac_addr(arp_packet->arp_sha);
	memcpy(&packet[18],arp_packet->arp_sha,ETH_ALEN);
	BOOST_LOG_TRIVIAL(trace) << "Target L3 addr " << format_ip_addr(arp_packet->arp_spa);
	memcpy(&packet[24],arp_packet->arp_spa,sizeof(in_addr_t));
	//writting data to stream
	response->sputn(packet,ARPHDR_LEN+ARPPAY_LEN);
	response->commit(ARPHDR_LEN+ARPPAY_LEN);
	const unsigned char* arp_raw_packet =
		boost::asio::buffer_cast<const unsigned char*>(response->data());
	BOOST_LOG_TRIVIAL(trace) << "Result ARP raw packet: " << hexa_print(arp_raw_packet,ARPHDR_LEN+ARPPAY_LEN);
	return response;
}

std::pair<boost::asio::streambuf*,size_t> ARPProtocolHandler::create_request(
		L2Socket* l2_socket,in_addr_t from,in_addr_t query){
	boost::asio::streambuf* response = new boost::asio::streambuf();
	BOOST_LOG_TRIVIAL(debug)
		<< "Writting arp request"
		<<" to: "
		<< format_ip_addr((uint8_t*)&query);
	ether_arp arp_packet;
	memset(&arp_packet,0,sizeof(struct ether_arp));
	//Setting destination mac


	//Swapping sender & target hw & protocol addresses (pointers)
	char packet[ARPHDR_LEN+ARPPAY_LEN];

	arp_packet.ea_hdr.ar_hrd = ARPHRD_ETHER;
	BOOST_LOG_TRIVIAL(trace)
		<< "arp: hrd (format of HWaddress) "
		<< boost::format("0x%+02x") % arp_packet.ea_hdr.ar_hrd;
	char_shift((unsigned char*)&arp_packet.ea_hdr.ar_hrd,sizeof(unsigned short int),(uint8_t*)&packet[0]);

	arp_packet.ea_hdr.ar_pro = ETHERTYPE_IP;
	BOOST_LOG_TRIVIAL(trace)
		<< "arp: pro (L3 protocol addresses id) "
		<< boost::format("0x%+02x") % arp_packet.ea_hdr.ar_pro;
	char_shift((unsigned char*)&arp_packet.ea_hdr.ar_pro,sizeof(unsigned short int),(uint8_t*)&packet[2]);

	arp_packet.ea_hdr.ar_pro = ETH_ALEN;
	BOOST_LOG_TRIVIAL(trace)
		<< "arp: hln (HW addr len (Ether len) "
		<< boost::format("0x%+02x") % (unsigned short)arp_packet.ea_hdr.ar_pro;
	memcpy(&packet[4],&arp_packet.ea_hdr.ar_pro,sizeof(unsigned char ));

	arp_packet.ea_hdr.ar_pln = sizeof(in_addr_t);
	BOOST_LOG_TRIVIAL(trace)
		<< "arp: pln (L3 proto len addr) "
		<< boost::format("0x%+02x") % (unsigned short)arp_packet.ea_hdr.ar_pln;
	memcpy(&packet[5],&arp_packet.ea_hdr.ar_pln,sizeof(unsigned char));

	arp_packet.ea_hdr.ar_op = ARPOP_REQUEST;
	BOOST_LOG_TRIVIAL(trace)
		<< "arp: op (ARP OP code) "
		<< boost::format("0x%+02x") % arp_packet.ea_hdr.ar_op;
	//memcpy(&packet[6],&arp_packet->ea_hdr.ar_pro,sizeof(unsigned short int));
	char_shift((unsigned char*)&arp_packet.ea_hdr.ar_op,sizeof(unsigned short int),(uint8_t*)&packet[6]);

	memcpy(arp_packet.arp_sha,l2_socket->mac_addr,ETH_ALEN);
	BOOST_LOG_TRIVIAL(trace) << "Sender HW addr " << format_mac_addr(arp_packet.arp_sha);
	memcpy(&packet[8],arp_packet.arp_sha,ETH_ALEN);

	memcpy(arp_packet.arp_spa,(uint8_t*)&from,sizeof(in_addr_t));//For consistency
	BOOST_LOG_TRIVIAL(trace) << "Sender L3 addr " << format_ip_addr(arp_packet.arp_spa);
	memcpy(&packet[14],arp_packet.arp_spa,sizeof(in_addr_t));

	//Target hardware address has been previously initialized to zero
	BOOST_LOG_TRIVIAL(trace) << "Target HW addr " << format_mac_addr(arp_packet.arp_tha);
	memcpy(&packet[18],arp_packet.arp_tha,ETH_ALEN);

	memcpy(arp_packet.arp_tpa,(uint8_t*)&query,sizeof(in_addr_t));//For consistency
	BOOST_LOG_TRIVIAL(trace) << "Target L3 addr " << format_ip_addr(arp_packet.arp_tpa);
	memcpy(&packet[24],arp_packet.arp_tpa,sizeof(in_addr_t));
	//writting data to stream
	response->sputn(packet,ARPHDR_LEN+ARPPAY_LEN);
	response->commit(ARPHDR_LEN+ARPPAY_LEN);
	const unsigned char* arp_raw_packet =
		boost::asio::buffer_cast<const unsigned char*>(response->data());
	BOOST_LOG_TRIVIAL(trace) << "Result ARP raw Query packet: " << hexa_print(arp_raw_packet,ARPHDR_LEN+ARPPAY_LEN);
	return std::pair<boost::asio::streambuf*,size_t>(response,sizeof(arp_packet));
}

