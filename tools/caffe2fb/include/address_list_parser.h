/*
 * address_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef ADDRESS_LIST_PARSER_H_
#define ADDRESS_LIST_PARSER_H_

#include "list_entry_parser.h"

namespace nvdla {

class AddressListParser: public ListEntryParser {
public:
	AddressListParser(NetParser* net);
	virtual ~AddressListParser();

	void  buildList();
	void* getList() const;


private:
	std::vector<ILoadable::AddressListEntry> mList;
};

} /* namespace nvdla */

#endif /* ADDRESS_LIST_PARSER_H_ */
