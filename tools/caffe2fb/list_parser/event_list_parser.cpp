/*
 * event_list_parser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#include "event_list_parser.h"

namespace nvdla {

EventListParser::EventListParser(NetParser* net) :
	ListEntryParser(net)
{
	// TODO Auto-generated constructor stub

}

EventListParser::~EventListParser() {
	// TODO Auto-generated destructor stub
}

const void* EventListParser::getList() const
{
	return (const void*)&mList;
}

void  EventListParser::buildList()
{
    //TODO
    log_info("Dummy EventListParser::buildList\n");
}


} /* namespace nvdla */
