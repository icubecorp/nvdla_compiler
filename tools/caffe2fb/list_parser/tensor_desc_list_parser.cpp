/*
 * tensor_desc_list_parser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#include "tensor_desc_list_parser.h"

namespace nvdla {

TensorDescListParser::TensorDescListParser(NetParser* net) :
	ListEntryParser(net)
{

}

TensorDescListParser::~TensorDescListParser()
{

}


const void* TensorDescListParser::getList() const
{
	return (const void*)&mList;
}

void TensorDescListParser::buildList()
{
	if (!mList.empty()) {
		printf("Warning: only build list once!\n");
		return;
	}

	ILoadable::TensorDescListEntry tensor_input;
	ILoadable::TensorDescListEntry tensor_output;

	//input tensor



	mList.push_back(tensor_input);
	mList.push_back(tensor_output);
}


} /* namespace nvdla */
