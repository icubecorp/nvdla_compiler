/*
 * memory_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "memory_list_parser.h"
#include <string>
using namespace std;

namespace nvdla {

static NvS32 mem_id = 1;
static NvS32 conv_id = 0;
static NvS32 sdp_id = 0;
static NvS32 pdp_id = 0;

#define MEM_ALIGNMENT_PAGE 4096
#define MEM_ALIGNMENT_LINE 32

static int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}

MemoryListParser::MemoryListParser(NetParser* net, TaskListParser *tlp) :
    ListEntryParser(net),
	mTaskListParser(tlp)
{

}

MemoryListParser::~MemoryListParser()
{

}

const void* MemoryListParser::getList() const 
{
	return (const void*)&mList;
}

TaskListParser* MemoryListParser::getTaskListParser(){
	return mTaskListParser;
}


NvU64 MemoryListParser::getInputMemSize(NvU32 w, NvU32 h, NvU32 c, NvU32 bpe, NvU32 align){
	
    NvU64 lineStride = roundUp(w * c * bpe, align);
    NvU64 surfaceStride = roundUp(lineStride * h, align);
    NvU64 size = roundUp(surfaceStride, align);
	return size;
}

NvU64 MemoryListParser::getCovlutionOutputMemSize(CONV_PAR_STR convpar){
	NvU64 output_height = (convpar.input_height + (convpar.padding_h << 1) - convpar.filter_width)/convpar.stripe_h + 1;
	NvU64 output_width = (convpar.input_width + (convpar.padding_w << 1) - convpar.filter_width)/convpar.stripe_w + 1;
	NvU64 size = output_height * output_width * roundUp(convpar.filter_numbers * convpar.byteperpixel, 128);
	return size;
}


void  MemoryListParser::layerInputParse(Layer* layer){
	nvdla::ILoadable::MemoryListEntry mle;
	NvS32 w;
	NvS32 h;
	NvS32 c;
	NvS32 bpe;
	union dla_layer_param_container layer_input_par;
    if(layer->nvdla_type != NvInput){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
    }
	//set alloc mem flag
	layer->src_mem_flag = 1;
	//get input parameters
	layer_input_par = layer->get_params();
	w = layer_input_par.nv_input_params.w;
	h = layer_input_par.nv_input_params.h;
	c = layer_input_par.nv_input_params.c;
	bpe = layer->get_bpe();
    //fill memory entry
	mle.id = mem_id;
	mle.alignment = MEM_ALIGNMENT_PAGE;
	mle.bind_id = 0;
	mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_INPUT;
	//mle.contents
	mle.offsets.push_back(0);
	mle.size  = getInputMemSize(w, h, c, bpe, 32);
	mle.tensor_desc_id = 0;
	//push mem entry to vector
	mList.push_back(mle);
	mem_id++;

	//write data back to layer
	//input
    layer->surface_desc.src_data.address = mem_id;
    layer->surface_desc.src_data.channel = c;
    layer->surface_desc.src_data.height = h;
    layer->surface_desc.src_data.line_stride = w * roundUp(c * bpe, MEM_ALIGNMENT_LINE);
    layer->surface_desc.src_data.plane_stride = 0;
    layer->surface_desc.src_data.size = mle.size;
    layer->surface_desc.src_data.surf_stride = layer->surface_desc.src_data.line_stride * h;
    //layer->surface_desc.src_data.type = 
    layer->surface_desc.src_data.width = w;
	//weight
	layer->surface_desc.weight_data.address = -1;
	layer->surface_desc.weight_data.channel = 0;
	layer->surface_desc.weight_data.height = 0;
	layer->surface_desc.weight_data.line_stride = 0;
	layer->surface_desc.weight_data.plane_stride = 0;
	layer->surface_desc.weight_data.size = 0;
	layer->surface_desc.weight_data.surf_stride = 0;
	layer->surface_desc.weight_data.width = 0;
	layer->surface_desc.weight_data.type = 0;

	//dst
    layer->surface_desc.dst_data.address = -1;
	layer->surface_desc.dst_data.channel = c;
	layer->surface_desc.dst_data.height = h;
	layer->surface_desc.dst_data.line_stride = w * roundUp(c * bpe, MEM_ALIGNMENT_LINE);;
	layer->surface_desc.dst_data.plane_stride = 0;
	layer->surface_desc.dst_data.size = mle.size;
	layer->surface_desc.dst_data.surf_stride = layer->surface_desc.src_data.surf_stride;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = w;
	return ;
	
}

void MemoryListParser::layerConvlutionParse(Layer* layer, Layer* pre_layer){
	nvdla::ILoadable::MemoryListEntry mle;
	CONV_PAR_STR convpar;
	string content;
	NvS32 bpe;
	union dla_layer_param_container layer_input_par;
        if(layer->nvdla_type != NvConv){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
        }
	bpe = layer->get_bpe();
	layer->weight_mem_flag = 1;
	//get input parameters
	convpar.input_width = pre_layer->surface_desc.dst_data.width;
	convpar.input_height = pre_layer->surface_desc.dst_data.height;
	convpar.input_channel = pre_layer->surface_desc.dst_data.channel;
	
	convpar.padding_w = layer_input_par.nv_conv_params.pad_w;
	convpar.padding_h = layer_input_par.nv_conv_params.pad_h;
	convpar.filter_width = layer_input_par.nv_conv_params.kernel_w;
	convpar.filter_height = layer_input_par.nv_conv_params.kernel_h;
	convpar.filter_channel = pre_layer->surface_desc.dst_data.channel;
	convpar.stripe_h = layer_input_par.nv_conv_params.stride_h;
	convpar.stripe_w = layer_input_par.nv_conv_params.stride_w;
	convpar.byteperpixel = layer->get_bpe();
	convpar.filter_numbers = layer_input_par.nv_conv_params.num_output;
	
	//---------fill weight mem entry---------
	mle.id = mem_id;
	mle.alignment = MEM_ALIGNMENT_PAGE;
	mle.bind_id = 0;
	mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
	//sprintf(content, "%d%s%d", conv_id, "conv_weight_", mem_id);
	conv_id++;
	mle.contents.push_back(content);
	mle.offsets.push_back(0);
	mle.size = getCovlutionOutputMemSize(convpar);
	mle.tensor_desc_id = 0;
	//push mem entry to vector
	mList.push_back(mle);
	mem_id++;

	//write data back to layer
	//input
    layer->surface_desc.src_data.address = pre_layer->surface_desc.dst_data.address;
    layer->surface_desc.src_data.channel = pre_layer->surface_desc.dst_data.channel;
    layer->surface_desc.src_data.height = pre_layer->surface_desc.dst_data.height;
    layer->surface_desc.src_data.line_stride = pre_layer->surface_desc.dst_data.line_stride;
    layer->surface_desc.src_data.plane_stride = pre_layer->surface_desc.dst_data.plane_stride;
    layer->surface_desc.src_data.size = pre_layer->surface_desc.dst_data.size;
    layer->surface_desc.src_data.surf_stride = pre_layer->surface_desc.dst_data.surf_stride;
    layer->surface_desc.src_data.type = pre_layer->surface_desc.dst_data.type;
    layer->surface_desc.src_data.width = pre_layer->surface_desc.dst_data.width;
	//weight
	layer->surface_desc.weight_data.address = mem_id;
	layer->surface_desc.weight_data.channel = convpar.filter_channel;
	layer->surface_desc.weight_data.height = convpar.filter_height;
	layer->surface_desc.weight_data.line_stride = 0;
	layer->surface_desc.weight_data.plane_stride = 0;
	layer->surface_desc.weight_data.size = convpar.filter_width * convpar.filter_height * convpar.filter_channel *bpe * convpar.filter_numbers;
	layer->surface_desc.weight_data.surf_stride = 0;
	layer->surface_desc.weight_data.width = convpar.filter_width;
	//layer->surface_desc.weight_data.type = 0;

	//dst
    layer->surface_desc.dst_data.address = -1;
	layer->surface_desc.dst_data.channel = convpar.filter_numbers;
	layer->surface_desc.dst_data.height = (convpar.input_height + (convpar.padding_h << 1) - convpar.filter_width)/convpar.stripe_h + 1;
	layer->surface_desc.dst_data.line_stride = roundUp(layer->surface_desc.dst_data.width * bpe, MEM_ALIGNMENT_LINE);
	layer->surface_desc.dst_data.plane_stride = 0;
	layer->surface_desc.dst_data.size = mle.size;
	layer->surface_desc.dst_data.surf_stride = layer->surface_desc.dst_data.line_stride * convpar.input_height;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = (convpar.input_width + (convpar.padding_w << 1) - convpar.filter_width)/convpar.stripe_w + 1;
	return ;
}

void MemoryListParser::layerSdpParse(Layer* layer, Layer* pre_layer){
	nvdla::ILoadable::MemoryListEntry mle;
	string content;
	union dla_layer_param_container layer_input_par;
        if(layer->nvdla_type != NvSDP){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
        }
	
	if(layer->get_action()){
		layer->weight_mem_flag = 1;

		//get layer parameters
		layer_input_par = layer->get_params();
		//w = layer_input_par.

		
		//---------fill weight mem entry---------
		mle.id = mem_id;
		mle.alignment = MEM_ALIGNMENT_PAGE;
		mle.bind_id = 0;
		mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
		mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
		//sprintf(content, "%s%d", "sdp_weight_", mem_id);
		mle.contents.push_back(content);
		mle.offsets.push_back(0);
		//mle.size 
		mle.tensor_desc_id = 0;
		//push mem entry to vector
		mList.push_back(mle);
		mem_id++;
		
	}
	
}


void  MemoryListParser::buildList()
{
	std::vector<Layer*> layers = mNetParserPtr->getLayers();
	nvdla::ILoadable::MemoryListEntry mle;
	Layer* layer = NULL;
        NvU32 index;	
	union dla_layer_param_container layer_parm;
	for(index = 0 ; index < layers.size(); index++){
		layer = layers[index];
		switch(layer->nvdla_type){
			case 0:
				break;
			case 1:
				//covolution
				break;
			case 2:
				//sdp
				break;
			case 3:
				//pdp
				break;
			case 4:
				//softmax
				break;
			default:
				printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__,__LINE__, layer->nvdla_type);
		}
	}         
}
} /* namespace nvdla */
