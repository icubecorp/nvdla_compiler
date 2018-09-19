/*
 * memory_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "memory_list_parser.h"

namespace nvdla {

MemoryListParser::MemoryListParser(NetParser* net) :
    ListEntryParser(net)
{

}

MemoryListParser::~MemoryListParser()
{

}

void* MemoryListParser::getList() const
{
	return &mList;
}

void  MemoryListParser::buildList()
{
	std::vector<Layer*> layers = mNetParserPtr->getLayers();
}


} /* namespace nvdla */
