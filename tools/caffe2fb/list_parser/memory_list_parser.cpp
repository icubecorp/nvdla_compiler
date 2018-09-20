/*
 * memory_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "memory_list_parser.h"

namespace nvdla {

static NvU32 mem_id = 0;
#define MEM_ALIGNMENT 4096

MemoryListParser::MemoryListParser(NetParser* net) :
    ListEntryParser(net)
{

}

MemoryListParser::~MemoryListParser()
{

}

const void* MemoryListParser::getList() const 
{
	return (const void*)&mList;
}

void  MemoryListParser::buildList()
{
	std::vector<Layer*> layers = mNetParserPtr->getLayers();
	nvdla::ILoadable::MemoryListEntry mle;
        NvU32 layer_index;	
	for(layer_index = 0 ; layer_index < layers.size(); layer_index++){
		if(layers[layer_index]->src_mem_flag){
			//fill src mem entry
			mle.id = mem_id;
			mle.alignment = MEM_ALIGNMENT;
			mle.bind_id = 0;
			mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
			mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_INPUT;
			//mle.contents
			//mle.offsets
			//mle.size
			mle.tensor_desc_id = 0;
			//push mem entry to vector
			mList.push_back(mle);
			mem_id++;
		}
		
		if(layers[layer_index]->weight_mem_flag){
			//fill weight mem
			mle.id = mem_id;
			mle.alignment = MEM_ALIGNMENT;
			mle.bind_id = 0;
			mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
			mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
			//mle.contents
			//mle.offsets
			//mle.size
			mle.tensor_desc_id = 0;
			//push mem entry to vector
			mList.push_back(mle);
			mem_id++;
		}
		
		if(layers[layer_index]->dst_mem_flag){
			//fill dst mem 
			mle.id = mem_id;
			mle.alignment = MEM_ALIGNMENT;
			mle.bind_id = 0;
			mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
			mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_OUTPUT;
			//mle.contents
			//mle.offsets
			//mle.size
			mle.tensor_desc_id = 0;
			//push mem entry to vector
			mList.push_back(mle);
			mem_id++;
		}
	}
}


} /* namespace nvdla */
