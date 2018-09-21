/*
 * address_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef ADDRESS_LIST_PARSER_H_
#define ADDRESS_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "memory_list_parser.h"

namespace nvdla {

class AddressListParser: public ListEntryParser {
public:
	AddressListParser(NetParser* net, MemoryListParser* mems);
	virtual ~AddressListParser();

	void  buildList();
	const void* getList() const;


private:
	std::vector<ILoadable::AddressListEntry> mList;

	MemoryListParser* mMemoryListParserPtr;
};

} /* namespace nvdla */

#endif /* ADDRESS_LIST_PARSER_H_ */
