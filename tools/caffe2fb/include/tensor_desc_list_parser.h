/*
 * tensor_desc_list_parser.h
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#ifndef TENSOR_DESC_LIST_PARSER_H_
#define TENSOR_DESC_LIST_PARSER_H_

#include "list_entry_parser.h"

namespace nvdla {

class TensorDescListParser: public ListEntryParser {
public:
	TensorDescListParser(NetParser* net);
	virtual ~TensorDescListParser();

	void  buildList();
	const void* getList() const;


private:
	std::vector<ILoadable::TensorDescListEntry> mList;
};

} /* namespace nvdla */

#endif /* TENSOR_DESC_LIST_PARSER_H_ */
