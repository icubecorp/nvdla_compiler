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
#include "priv/loadable_generated.h"
#include "debug.h"
namespace nvdla {

enum LIST_PARSER_TYPE {
	TASK_LIST_PARSER,
	SUBMIT_LIST_PARSER,
	MEMORY_LIST_PARSER,
	ADDRESS_LIST_PARSER,
	EVENT_LIST_PARSER,
	TENSOR_DESC_LIST_PARSER,
	SYMBOL_LIST_PARSER,
	RELOC_LIST_PARSER,
	LIST_PARSER_NUM
};

class ListEntryParser {
public:
	ListEntryParser(NetParser* net);
	virtual ~ListEntryParser();

	virtual void  buildList() = 0;
	virtual const void* getList() const = 0;

protected:
	NetParser* mNetParserPtr;
};

} /* namespace nvdla */

#endif /* LIST_ENTRY_PARSER_H_ */
