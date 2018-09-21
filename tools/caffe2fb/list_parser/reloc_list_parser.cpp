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

}



} /* namespace nvdla */
