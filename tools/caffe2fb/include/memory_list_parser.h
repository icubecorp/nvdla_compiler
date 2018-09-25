/*
 * memory_list_parser.h
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#ifndef MEMORY_LIST_PARSER_H_
#define MEMORY_LIST_PARSER_H_

#include "list_entry_parser.h"
#include "task_list_parser.h"

namespace nvdla {

class TaskListParser;

typedef struct ConvolutionPar{
	NvS32 input_width;
	NvS32 input_height;
	NvS32 input_channel;

	NvS32 filter_width;
	NvS32 filter_height;
	NvS32 filter_channel;
	NvS32 filter_numbers;

	NvS32 padding_w;
	NvS32 padding_h;
	NvS32 stripe_h;
	NvS32 stripe_w;
	NvS32 byteperpixel;
}CONV_PAR_STR;

class MemoryListParser: public ListEntryParser {
public:
	MemoryListParser(NetParser* net, TaskListParser *tlp);
	virtual ~MemoryListParser();

	void  buildList();
	const void* getList() const;
	TaskListParser* getTaskListParser();
	NvU64 getInputMemSize(NvU32 w, NvU32 h, NvU32 c, NvU32 bpe, NvU32 align);
	NvU64 getCovlutionOutputMemSize(CONV_PAR_STR* convpar);

	void layerInputParse(Layer* layer);
	void layerConvlutionParse(Layer* layer, Layer* pre_layer);
	void layerSdpParse(Layer* layer, Layer* pre_layer);
	void layerPdpParse(Layer* layer, Layer* pre_layer);
	void layerSoftmaxParse(Layer* layer, Layer* pre_layer);
	void taskTypeParse(ILoadable::Interface task_type);
	void debugMemList(void);

private:
	TaskListParser* mTaskListParser;
	std::vector<ILoadable::MemoryListEntry> mList;
};

} /* namespace nvdla */

#endif /* MEMORY_LIST_PARSER_H_ */
