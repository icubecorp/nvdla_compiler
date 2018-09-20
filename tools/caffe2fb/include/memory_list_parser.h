/*
 * memory_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef MEMORY_LIST_PARSER_H_
#define MEMORY_LIST_PARSER_H_

#include "list_entry_parser.h"

namespace nvdla {

class MemoryListParser: public ListEntryParser {
public:
	MemoryListParser(NetParser* net);
	virtual ~MemoryListParser();

	void  buildList();
	const void* getList() const;

private:
	std::vector<ILoadable::MemoryListEntry> mList;
};

} /* namespace nvdla */

#endif /* MEMORY_LIST_PARSER_H_ */
