/*
 * symbol_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "symbol_list_parser.h"

namespace nvdla {

SymbolListParser::SymbolListParser(MemoryListParser* memory_parser) :
	ListEntryParser(),
	mMemoryListParserPtr(memory_parser)
{

}

SymbolListParser::~SymbolListParser() {

}

void* SymbolListParser::getList() const {
	return (void*)&mList;
}

void SymbolListParser::buildList() {

	int layer_num = 0;

	for (int i=0; i<layer_num; i++) {

		ILoadable::TaskListEntry task;

		task.id = 0;
		task.interface = ILoadable::Interface_DLA1;
		task.instance = -1;

		// pre-action
		for(;;) {
			task.preactions.push_back(0);
		}

		// post-action
		for(;;) {
			task.postactions.push_back(0);
		}

		//task.address_list = ?

		mList.push_back(task);
	}
}

} /* namespace nvdla */
