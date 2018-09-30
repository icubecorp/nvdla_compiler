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
#include "list_entry_parser.h"
#include "nvdla_interface.h"
#include "half.h"

namespace nvdla {

class SymbolListParser: public ListEntryParser {
public:
static int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}
	SymbolListParser(NetParser* net, MemoryListParser* memory_parser, TaskListParser* task_parser);
	virtual ~SymbolListParser();

	void  buildList();
	const void* getList() const;
    void fill_weight_blobs(std::vector<priv::Loadable::Symbol> *mlist, NetParser* net,\
            MemoryListParser* memory_parser);
    void fill_taskinfo_blobs(void);
    void fill_nvdla_taskinfo_blobs(ILoadable::TaskListEntry task_entry);
    void fill_dla_dep_graph_blob(ILoadable::TaskListEntry task_entry, priv::Loadable::Symbol *dep_graph_blob);
    void set_default_dep_graph(struct dla_common_op_desc *dep_graph);
    int32_t find_next_layer_index(int32_t cur_layer, layer_type type, int32_t last_layer);
    int32_t find_first_layer_index(int32_t first_layer, uint8_t type, int32_t last_layer);
    void fill_emu_taskinfo_blobs(ILoadable::TaskListEntry task_entry);
    void  fill_emu_taskinfo_blob(void);
    void* fill_conv_weight_data(Layer * layer);
    void* fill_bias_weight_data(Layer * layer);
    void dump_blobs_info(void);
	void debugEmuBlobInfo(void);
    enum weight_format {WEIGHT_DIRECT_CONV = 1, WEIGHT_BIAS = 2,WEIGHT_CAFFEMODE = 3};

    struct nvdla_meta_data
    {
        weight_format data_format;
        int width;
        int height;
        int channel;
        int kernel_num;
        int atomic_size;
        int bpe;
        int dynamic_atomic_size;
    };
    int get_offset(uint16_t k, uint16_t c, uint16_t h, uint16_t w, struct nvdla_meta_data meta_data);
private:

	std::vector<priv::Loadable::Symbol> mList;
	MemoryListParser* mMemoryListParserPtr;
    TaskListParser* mTaskListParserPtr;

};





} /* namespace nvdla */

#endif /* SYMBOL_LIST_PARSER_H_ */
