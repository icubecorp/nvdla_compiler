/*
 * reloc_list_parser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#include "reloc_list_parser.h"

namespace nvdla {

RelocListParser::RelocListParser(NetParser* net) :
	ListEntryParser(net)
{

}

RelocListParser::~RelocListParser()
{
}

const void* RelocListParser::getList() const
{
	return (const void*)&mList;
}

void  RelocListParser::buildList()
{
    //TODO
    log_info("Dummy RelocListParser::buildList\n");
}


void  RelocListParser::dumpList()
{
    //TODO
    log_info("Dummy RelocListParser::dumpList\n");
}

} /* namespace nvdla */
