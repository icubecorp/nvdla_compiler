/*
 * tensor_desc_list_parser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#ifndef TENSOR_DESC_LIST_PARSER_H_
#define TENSOR_DESC_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "memory_list_parser.h"
#include "nvdla_interface.h"
namespace nvdla {

class TensorDescListParser: public ListEntryParser {
public:
	TensorDescListParser(NetParser* net, MemoryListParser* mMemoryListParser);
	virtual ~TensorDescListParser();

	void  buildList();
	const void* getList() const;
    void dump_tensor_info(void);


private:
	std::vector<ILoadable::TensorDescListEntry> mList;
    MemoryListParser* mMemoryListParserPtr;
};

} /* namespace nvdla */

#endif /* TENSOR_DESC_LIST_PARSER_H_ */
