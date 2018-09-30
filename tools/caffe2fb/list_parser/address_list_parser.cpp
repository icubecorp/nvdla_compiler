/*
 * address_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "address_list_parser.h"
using std::string;

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

	for (unsigned int i=0; i<mems.size(); i++) {
		ILoadable::AddressListEntry addr;

		addr.id = i;
		addr.mem_id = mems[i].id;
        if( !mems[i].contents.empty() && mems[i].contents[0].find("task_") != string::npos)
            addr.size = 0;
        else
		    addr.size = mems[i].size;
		addr.offset = 0;

		mList.push_back(addr);
	}

}

void AddressListParser::dumplist(void){
    debug_info("enter %s line=%d\n",__FUNCTION__,__LINE__);
    for(unsigned int i = 0; i < mList.size(); i++){
        debug_info("id=%d\n",mList[i].id);
        debug_info("memid=%d\n",mList[i].mem_id);
        debug_info("size=%d\n",mList[i].size);
        debug_info("offset=%d\n",mList[i].offset);
    }

}


} /* namespace nvdla */
