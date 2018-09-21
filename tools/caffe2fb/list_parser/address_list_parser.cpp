/*
 * address_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "address_list_parser.h"

namespace nvdla {

AddressListParser::AddressListParser(NetParser* net, MemoryListParser* mems) :
	ListEntryParser(net),
	mMemoryListParserPtr(mems)
{

}

AddressListParser::~AddressListParser() {
}

const void* AddressListParser::getList() const
{
	return (const void*)&mList;
}

void AddressListParser::buildList()
{
	const std::vector<ILoadable::MemoryListEntry> mems =
			*(const std::vector<ILoadable::MemoryListEntry>*)mMemoryListParserPtr->getList();

	for (int i=0; i<mems.size(); i++) {
		ILoadable::AddressListEntry addr;

		addr.id = i;
		addr.mem_id = mems[i].id;
		addr.size = mems[i].size;
		addr.offset = 0;

		mList.push_back(addr);
	}

}

} /* namespace nvdla */
