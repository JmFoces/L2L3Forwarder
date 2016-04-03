/*
 * UserInterface.h
 *
 *  Created on: Feb 24, 2016
 *      Author: xshell
 */

#ifndef USERINTERFACE_H_
#define USERINTERFACE_H_
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
class UserInterface {
public:
	static const char OP_MODE_ROUTER='r';
	static const char OP_MODE_SWITCH='s';
	static const char OP_CODE_SHOW_TABLES='t';
	static const char OP_CODE_ADD_IFACE='a';
	static const char OP_CODE_QUIT='q';
	UserInterface();
	virtual ~UserInterface();
	void run();
	void load_router(boost::property_tree::ptree pt);
	void load_switch(boost::property_tree::ptree pt);
	void run_switch();
	void run_router();

};

#endif /* USERINTERFACE_H_ */
