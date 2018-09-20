/*
 * symbol_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef SYMBOL_LIST_PARSER_H_
#define SYMBOL_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "priv/Loadable.h"
#include "memory_list_parser.h"

namespace nvdla {

class SymbolListParser: public ListEntryParser {
public:
	SymbolListParser(NetParser* net, MemoryListParser* memory_parser);
	virtual ~SymbolListParser();

	void  buildList();
	const void* getList() const;

private:

	std::vector<priv::Loadable::Symbol> mList;
	MemoryListParser* mMemoryListParserPtr;
};

} /* namespace nvdla */

#endif /* SYMBOL_LIST_PARSER_H_ */
