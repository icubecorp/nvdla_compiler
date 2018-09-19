/*
 * list_entry_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef LIST_ENTRY_PARSER_H_
#define LIST_ENTRY_PARSER_H_

#include "nvdla/ILoadable.h"
#include "net_parser.h"

namespace nvdla {

class ListEntryParser {
public:
	ListEntryParser(NetParser* net);
	virtual ~ListEntryParser();

	virtual void  buildList() = 0;
	virtual void* getList() const = 0;


protected:
	NetParser* mNetParserPtr;
};

} /* namespace nvdla */

#endif /* LIST_ENTRY_PARSER_H_ */
