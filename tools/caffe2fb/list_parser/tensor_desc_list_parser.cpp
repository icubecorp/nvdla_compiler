/*
 * tensor_desc_list_parser.cpp
 *
 *  Created on: Sep 20, 2018
 *      Author: jiqianxiang
 */

#include "tensor_desc_list_parser.h"
#include "nvdla/IRuntime.h"
namespace nvdla {

TensorDescListParser::TensorDescListParser(NetParser* net,      MemoryListParser* memory_parser) :
	ListEntryParser(net),
    mMemoryListParserPtr(memory_parser)
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

    std::vector<Layer*> layers = mNetParserPtr->getLayers();
    Layer * layer;
    const std::vector<ILoadable::MemoryListEntry> *mem_list =  \
            (const std::vector<ILoadable::MemoryListEntry> *)mMemoryListParserPtr->getList();
    ILoadable::MemoryListEntry mem_entry;
	ILoadable::TensorDescListEntry tensor_input;
	ILoadable::TensorDescListEntry tensor_output;
    memset(&tensor_input, 0, sizeof(ILoadable::TensorDescListEntry));
    memset(&tensor_output, 0, sizeof(ILoadable::TensorDescListEntry));
    int layer_index = 0;
    int memid = 0;
	//input tensor
	//first layer src mem  should be for the input tensor
    layer_index = 0;
    layer = layers[layer_index];
    if(layer->src_mem_flag){
        memid = layer->surface_desc.src_data.address;
        mem_entry = (*mem_list)[memid];
        if(mem_entry.flags == (nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_INPUT)){
            tensor_input.name = "data";
            tensor_input.id = 0;
            tensor_input.memId = memid;
            tensor_input.size = mem_entry.size;
            tensor_input.offset = 0;
            tensor_input.dims.c = layer->surface_desc.src_data.channel;
            tensor_input.dims.h = layer->surface_desc.src_data.height;
            tensor_input.dims.w = layer->surface_desc.src_data.width;
            tensor_input.dims.n = 1;
            tensor_input.dataFormat = 3;
            tensor_input.dataType = nvdla::loadable::DataType_HALF;
            tensor_input.dataCategory = nvdla::loadable::DataCategory_FEATURE;
            tensor_input.pixelFormat = TENSOR_PIXEL_FORMAT_FEATURE;
            tensor_input.pixelMapping = 0;
            tensor_input.stride[0] = layer->get_bpe();
            tensor_input.stride[1] = layer->surface_desc.src_data.line_stride;
            tensor_input.stride[2] = layer->surface_desc.src_data.surf_stride;
            tensor_input.stride[3] = layer->surface_desc.src_data.plane_stride;
        }
        else
            log_error("first layer src mem flag is wrong\n");
            
    }
    else
        log_error("first layer not allocate src mem");

    //output tensor
    //last layer dst mem should be for the output tensor
    layer_index = layers.size() - 1;
    layer = layers[layer_index];
    debug_info("layer_Index=%d\n",layer_index);
    if(layer->dst_mem_flag){
        memid = layer->surface_desc.dst_data.address;
        debug_info("%s memid=%d\n",__FUNCTION__,memid);
        mem_entry = (*mem_list)[memid];
        if(mem_entry.flags == (nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_OUTPUT)){
            tensor_output.name = "probe";
            tensor_output.id = 1;
            tensor_output.memId = memid;
            tensor_output.size = mem_entry.size;
            tensor_output.offset = 0;
            tensor_output.dims.c = layer->surface_desc.dst_data.channel;
            tensor_output.dims.h = layer->surface_desc.dst_data.height;
            tensor_output.dims.w = layer->surface_desc.dst_data.width;
            tensor_output.dims.n = 1;
            tensor_output.dataFormat = 3;
            tensor_output.dataType = nvdla::loadable::DataType_HALF;
            tensor_output.dataCategory = nvdla::loadable::DataCategory_FEATURE;
            tensor_output.pixelFormat = TENSOR_PIXEL_FORMAT_FEATURE;
            tensor_output.pixelMapping = 0;
            tensor_output.stride[0] = layer->get_bpe();
            tensor_output.stride[1] = layer->surface_desc.dst_data.line_stride;
            tensor_output.stride[2] = layer->surface_desc.dst_data.surf_stride;
            tensor_output.stride[3] = layer->surface_desc.dst_data.plane_stride;
        }
        else
            log_error("last layer dst mem flag is wrong\n");
            
    }
    else
        log_error("last layer not allocate dst mem");
	mList.push_back(tensor_input);
	mList.push_back(tensor_output);
}


void TensorDescListParser::dumpList(){
    debug_info("enter TensorDescListParser %s line=%d\n",__FUNCTION__,__LINE__);
    for(unsigned int i = 0; i < mList.size(); i++){
        ILoadable::TensorDescListEntry tensor = mList[i];
        debug_info("name=%s\n",tensor.name.c_str());
        debug_info("id=%d\n",tensor.id);
        debug_info("memId=%d\n",tensor.memId);
        debug_info("size=%d\n",tensor.size);

        debug_info("offset=%d\n",tensor.offset);
        debug_info("dims.n=%d\n",tensor.dims.n);
        debug_info("dims.c=%d\n",tensor.dims.c);
        debug_info("dims.h=%d\n",tensor.dims.h);
        debug_info("dims.w=%d\n",tensor.dims.w);
        
        debug_info("dataFormat=%d\n",tensor.dataFormat);
        debug_info("dataCategory=%d\n",tensor.dataCategory);
        debug_info("dataType=%d\n",tensor.dataType);
        debug_info("pixelFormat=%d\n",tensor.pixelFormat);
        debug_info("pixelMapping=%d\n",tensor.pixelMapping);
        
        debug_info("stride[0]=%d\n",tensor.stride[0]);
        debug_info("stride[1]=%d\n",tensor.stride[1]);
        debug_info("stride[2]=%d\n",tensor.stride[2]);
        debug_info("stride[3]=%d\n",tensor.stride[3]);
    }
    

}

} /* namespace nvdla */
