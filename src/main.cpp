/*
 * main.cpp
 *
 *  Created on: Jan 9, 2016
 *      Author: xshell
 */

#include <cstddef>
#include <iostream>
#include <map>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <sockets/L3Socket.h>
#include <sockets/SwitchSocket.h>
#include <tables/RoutingTable.h>
#include <UserInterface.h>
using namespace std;

void init_logging(boost::log::trivial::severity_level severity)
{
    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= severity
    );
}


int main(){
	init_logging(boost::log::trivial::trace);
	UserInterface ui;
	ui.run();
}
