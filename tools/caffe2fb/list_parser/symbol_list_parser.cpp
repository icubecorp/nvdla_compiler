/*
 * symbol_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "symbol_list_parser.h"

namespace nvdla {

SymbolListParser::SymbolListParser(NetParser* net, MemoryListParser* memory_parser) :
	ListEntryParser(net),
	mMemoryListParserPtr(memory_parser)
{

}

SymbolListParser::~SymbolListParser() {

}

const void* SymbolListParser::getList() const {
	return (const void*)&mList;
}

void SymbolListParser::buildList() {

}

} /* namespace nvdla */
