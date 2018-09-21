/*
 * reloc_list_parser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#ifndef RELOC_LIST_PARSER_H_
#define RELOC_LIST_PARSER_H_

#include "list_entry_parser.h"

namespace nvdla {

class RelocListParser: public ListEntryParser {
public:
	RelocListParser(NetParser* net);
	virtual ~RelocListParser();

	void  buildList();
	const void* getList() const;

private:
	std::vector<ILoadable::RelocEntry> mList;
};

} /* namespace nvdla */

#endif /* RELOC_LIST_PARSER_H_ */
