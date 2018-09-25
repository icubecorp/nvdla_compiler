/*
 * memory_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "memory_list_parser.h"
#include "debug.h"
#include <string>
#include <sstream>
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
    //printf("%s, %d\n", __FUNCTION__, __LINE__);
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
	
    NvU64 lineStride = w * roundUp(c * bpe, align);
    NvU64 surfaceStride = roundUp(lineStride * h, align);
    NvU64 size = roundUp(surfaceStride, align);
    return size;
}

NvU64 MemoryListParser::getCovlutionOutputMemSize(CONV_PAR_STR* convpar){
        debug_info("%s, %d, input_height = %d, padding_h = %d, filter_height = %d, stripe_h = %d\n", __FUNCTION__, __LINE__, convpar->input_height, convpar->padding_h, convpar->filter_height, convpar->stripe_h);
	NvU64 output_height = (convpar->input_height + (convpar->padding_h << 1) - convpar->filter_height)/convpar->stripe_h + 1;
        debug_info("%s, %d, output_height = %lld\n", __FUNCTION__, __LINE__, output_height);
	NvU64 output_width = (convpar->input_width + (convpar->padding_w << 1) - convpar->filter_width)/convpar->stripe_w + 1;
        debug_info("%s, %d, output_width = %lld\n", __FUNCTION__, __LINE__, output_width);
	NvU64 size = output_height * output_width * roundUp(convpar->filter_numbers * convpar->byteperpixel, 32);
        debug_info("%s, %d, size = %lld\n", __FUNCTION__, __LINE__, size);
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
        debug_info("%s, %d, w = %d, h = %d, c = %d, bpe = %d\n", __FUNCTION__, __LINE__, w, h, c, bpe);
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
	stringstream content;
	NvS32 bpe;
	union dla_layer_param_container layer_input_par;
        if(layer->nvdla_type != NvConv){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
        }
        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	bpe = layer->get_bpe();
	layer->weight_mem_flag = 1;
	//get input parameters
        layer_input_par = layer->get_params();
	convpar.input_width = pre_layer->surface_desc.dst_data.width;
	convpar.input_height = pre_layer->surface_desc.dst_data.height;
	convpar.input_channel = pre_layer->surface_desc.dst_data.channel;
        	
        debug_info("%s, %d, convpar.input_width = %d, convpar.input_height = %d, convpar.input_channel = %d\n", __FUNCTION__, __LINE__, convpar.input_width, convpar.input_height, convpar.input_channel);
	convpar.padding_w = layer_input_par.nv_conv_params.pad_w;
	convpar.padding_h = layer_input_par.nv_conv_params.pad_h;
        debug_info("%s, %d, convpar.padding_w = %d, convpar.padding_h = %d\n", __FUNCTION__, __LINE__, convpar.padding_w, convpar.padding_h);
	convpar.filter_width = layer_input_par.nv_conv_params.kernel_w;
	convpar.filter_height = layer_input_par.nv_conv_params.kernel_h;
	convpar.filter_channel = pre_layer->surface_desc.dst_data.channel;
	convpar.stripe_h = layer_input_par.nv_conv_params.stride_h;
	convpar.stripe_w = layer_input_par.nv_conv_params.stride_w;
	convpar.byteperpixel = layer->get_bpe();
	convpar.filter_numbers = layer_input_par.nv_conv_params.num_output;
        if((convpar.filter_width == -1) && (convpar.filter_height == -1)){
            convpar.filter_width = pre_layer->surface_desc.dst_data.width;
            convpar.filter_height = pre_layer->surface_desc.dst_data.height;
            convpar.stripe_h = 1;
            convpar.stripe_w = 1;
            convpar.filter_channel = pre_layer->surface_desc.dst_data.channel;
            convpar.padding_w = 0;
            convpar.padding_h = 0;
        }
        debug_info("%s, %d, filter_width =%d, filter_height = %d, filter_channel = %d, stripe_h = %d, stripe_w = %d, filter_numbers = %d\n", __FUNCTION__, __LINE__, convpar.filter_width, convpar.filter_height, convpar.filter_channel, convpar.stripe_h, convpar.stripe_w, convpar.filter_numbers);	
        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	//---------fill weight mem entry---------
	mle.id = mem_id;
	mle.alignment = MEM_ALIGNMENT_PAGE;
	mle.bind_id = 0;
	mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	content << conv_id <<"_conv_weight_" << mem_id << endl;

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	conv_id++;
	mle.contents.push_back(content.str());

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	mle.offsets.push_back(0);

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	mle.size = roundUp(convpar.filter_width * convpar.filter_height * convpar.filter_channel *bpe * convpar.filter_numbers, 128);

        debug_info("%s, %d, mle.size = %lld\n", __FUNCTION__, __LINE__, mle.size);
	mle.tensor_desc_id = 0;
	//push mem entry to vector
        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
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

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	//weight
	layer->surface_desc.weight_data.address = mem_id;
	layer->surface_desc.weight_data.channel = convpar.filter_channel;
	layer->surface_desc.weight_data.height = convpar.filter_height;
	layer->surface_desc.weight_data.line_stride = 0;
	layer->surface_desc.weight_data.plane_stride = 0;
	layer->surface_desc.weight_data.size = mle.size;
	layer->surface_desc.weight_data.surf_stride = 0;
	layer->surface_desc.weight_data.width = convpar.filter_width;
	//layer->surface_desc.weight_data.type = 0;

        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	//dst
        layer->surface_desc.dst_data.address = -1;
	layer->surface_desc.dst_data.channel = convpar.filter_numbers;
	layer->surface_desc.dst_data.height = (convpar.input_height + (convpar.padding_h << 1) - convpar.filter_width)/convpar.stripe_h + 1;
	layer->surface_desc.dst_data.line_stride = roundUp(layer->surface_desc.dst_data.width * bpe, MEM_ALIGNMENT_LINE);
	layer->surface_desc.dst_data.plane_stride = 0;
	layer->surface_desc.dst_data.size = getCovlutionOutputMemSize(&convpar);
	layer->surface_desc.dst_data.surf_stride = layer->surface_desc.dst_data.line_stride * convpar.input_height;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = (convpar.input_width + (convpar.padding_w << 1) - convpar.filter_width)/convpar.stripe_w + 1;
	return ;
}

void MemoryListParser::layerSdpParse(Layer* layer, Layer* pre_layer){
	nvdla::ILoadable::MemoryListEntry mle;
	stringstream content;
	NvS32 bpe;
        if(layer->nvdla_type != NvSDP){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
        }
	bpe = layer->get_bpe();
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
	
	if(SDP_ACTION_ADD_BIAS == layer->get_action()){
	   layer->weight_mem_flag = 1;
           //weight
	   layer->surface_desc.weight_data.address = mem_id;
	   layer->surface_desc.weight_data.channel = 1;
	   layer->surface_desc.weight_data.height = 1;
	   layer->surface_desc.weight_data.line_stride = 1;
	   layer->surface_desc.weight_data.plane_stride = 1;
	   layer->surface_desc.weight_data.size = layer->surface_desc.src_data.channel * bpe;
	   layer->surface_desc.weight_data.surf_stride = 1;
	   layer->surface_desc.weight_data.width = 1;
                
           //fill weight mem
	   mle.id = mem_id;
	   mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
	   content << sdp_id << "_sdp_weight_" << mem_id <<endl;
	   mle.contents.push_back(content.str());
	   mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	   mle.tensor_desc_id = 0;
	   mle.size = layer->surface_desc.weight_data.size; 
	   mle.bind_id = 0;
	   mle.alignment = MEM_ALIGNMENT_PAGE;
	   mList.push_back(mle);
	   mem_id++;
	}else{
	   //weight
	   layer->surface_desc.weight_data.address = -1;
	   layer->surface_desc.weight_data.channel = 0;
	   layer->surface_desc.weight_data.height = 0;
	   layer->surface_desc.weight_data.line_stride = 0;
	   layer->surface_desc.weight_data.plane_stride = 0;
	   layer->surface_desc.weight_data.size = 0;
	   layer->surface_desc.weight_data.surf_stride = 0;
	   layer->surface_desc.weight_data.width = 0;
	}
	//dst
        layer->dst_mem_flag  = 1;
	layer->surface_desc.dst_data.address = mem_id;
	layer->surface_desc.dst_data.channel = layer->surface_desc.src_data.channel;
	layer->surface_desc.dst_data.height = layer->surface_desc.src_data.height;
	layer->surface_desc.dst_data.line_stride = layer->surface_desc.src_data.line_stride;
	layer->surface_desc.dst_data.plane_stride = layer->surface_desc.src_data.plane_stride;
	layer->surface_desc.dst_data.size = layer->surface_desc.src_data.size;
	layer->surface_desc.dst_data.surf_stride = layer->surface_desc.src_data.surf_stride;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = layer->surface_desc.src_data.width;
	//---------fill dst mem entry----------------
	mle.id = mem_id;
	mle.alignment = MEM_ALIGNMENT_PAGE;
	mle.bind_id = 0;
	mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
	mle.offsets.push_back(0);
        while(mle.contents.size()){
	    mle.contents.pop_back();
        }
	mle.size = layer->surface_desc.dst_data.size;
	mle.tensor_desc_id = 0;
	//push mem entry to vector
	mList.push_back(mle);
	mem_id++;
        sdp_id++;
	return ;
}

void MemoryListParser::layerPdpParse(Layer* layer, Layer* pre_layer){
	nvdla::ILoadable::MemoryListEntry mle;
	stringstream content;
	union dla_layer_param_container layer_input_par;
	NvS32 w;
	NvS32 h;
	NvS32 c;
	NvS32 pad_bottom;
	NvS32 pad_left;
	NvS32 pad_right;
	NvS32 pad_top;
	//NvS32 pad_mode;
	//NvS32 global_pooling;
	//NvS32 pooling_type;
	NvS32 stripe_h;
	NvS32 stripe_w;
	NvS32 pbe;
	
        if(layer->nvdla_type != NvPDP){
           printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
	   return ;
        }
	layer->dst_mem_flag = 1;
	pbe = layer->get_bpe();
	layer_input_par = layer->get_params();
	//global_pooling = layer_input_par.pdp_params.global_pooling;
	h = layer_input_par.pdp_params.kernel_h;
	w = layer_input_par.pdp_params.kernel_w;
	c = pre_layer->surface_desc.dst_data.channel;
	pad_bottom = layer_input_par.pdp_params.pad_bottom;
	pad_left = layer_input_par.pdp_params.pad_left;
	//pad_mode = layer_input_par.pdp_params.pad_mode;
	pad_right = layer_input_par.pdp_params.pad_right;
	pad_top = layer_input_par.pdp_params.pad_top;
	//pooling_type = layer_input_par.pdp_params.pooling_type;
	stripe_h = layer_input_par.pdp_params.stride_h;
	stripe_w = layer_input_par.pdp_params.stride_w;
        
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
	layer->surface_desc.weight_data.address = -1;
	layer->surface_desc.weight_data.channel = c;
	layer->surface_desc.weight_data.height = h;
	layer->surface_desc.weight_data.line_stride = 0;
	layer->surface_desc.weight_data.plane_stride = 0;
	layer->surface_desc.weight_data.size = 0;
	layer->surface_desc.weight_data.surf_stride = 0;
	layer->surface_desc.weight_data.width = w;
	//dst
	layer->surface_desc.dst_data.address = mem_id;
	layer->surface_desc.dst_data.channel = layer->surface_desc.src_data.channel;
	layer->surface_desc.dst_data.height = (layer->surface_desc.src_data.height + pad_top + pad_bottom - h)/stripe_h + 1;
	layer->surface_desc.dst_data.plane_stride = 0;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = (layer->surface_desc.src_data.width + pad_left + pad_right - w)/stripe_w + 1;
	layer->surface_desc.dst_data.line_stride = roundUp(layer->surface_desc.dst_data.width *pbe * layer->surface_desc.dst_data.channel, 32);
	layer->surface_desc.dst_data.surf_stride = layer->surface_desc.dst_data.line_stride * layer->surface_desc.dst_data.height;
	layer->surface_desc.dst_data.size = layer->surface_desc.dst_data.width * layer->surface_desc.dst_data.height * roundUp(layer->surface_desc.dst_data.channel * pbe, 32) * 1;
	//---------fill weight mem entry----------------
	mle.id = mem_id;
	mle.alignment = MEM_ALIGNMENT_PAGE;
	mle.bind_id = 0;
	mle.domain = nvdla::ILoadable::MemoryDomain_SYSMEM;
	mle.flags = nvdla::ILoadable::MemoryFlags_ALLOC | nvdla::ILoadable::MemoryFlags_SET;
	mle.offsets.push_back(0);
	mle.size = layer->surface_desc.dst_data.size; 
	mle.tensor_desc_id = 0;
	//push mem entry to vector
	mList.push_back(mle);
	mem_id++;
        return;
	
}

void MemoryListParser::layerSoftmaxParse(Layer* layer, Layer* pre_layer){
	nvdla::ILoadable::MemoryListEntry mle;
	string content;
	union dla_layer_param_container layer_input_par;
        if(layer->nvdla_type != NvSoftmax){
		printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__, __LINE__, layer->nvdla_type);
		return ;
        }
	layer_input_par = layer->get_params();
	//layer_input_par.nv_softmax_params.axis;

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
	layer->surface_desc.weight_data.address = -1;
	layer->surface_desc.weight_data.channel = 0;
	layer->surface_desc.weight_data.height = 0;
	layer->surface_desc.weight_data.line_stride = 0;
	layer->surface_desc.weight_data.plane_stride = 0;
	layer->surface_desc.weight_data.size = 0;
	layer->surface_desc.weight_data.surf_stride = 0;
	layer->surface_desc.weight_data.width = 0;
	//dst
	layer->surface_desc.dst_data.address = -1;
	layer->surface_desc.dst_data.channel = 1;
	layer->surface_desc.dst_data.height = 1;
	layer->surface_desc.dst_data.plane_stride = 0;
	//layer->surface_desc.dst_data.type = 0;
	layer->surface_desc.dst_data.width = 0;
	layer->surface_desc.dst_data.line_stride = 0;
	layer->surface_desc.dst_data.surf_stride = 0;
	layer->surface_desc.dst_data.size =0;
	return ;
}

void MemoryListParser::taskTypeParse(ILoadable::Interface task_type){
	if(task_type == ILoadable::Interface_DLA1){
		//alloc mem for task
	}
	return ;
}

void  MemoryListParser::buildList()
{
	std::vector<Layer*> layers = mNetParserPtr->getLayers();
	nvdla::ILoadable::MemoryListEntry mle;
	Layer* layer = NULL;
	Layer* pre_layer = NULL;
        NvU32 index;
	std::vector<ILoadable::Interface> task_type_list;
        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	//alloc mem for layers
	for(index = 0 ; index < layers.size(); index++){
		layer = layers[index];
		if(index > 0){
			pre_layer = layers[index-1];
		}
                debug_info("%s, %d, index= %d, layer->nvdla_type = %d\n", __FUNCTION__, __LINE__, index, layer->nvdla_type);
		switch(layer->nvdla_type){
			case NvInput:
				layerInputParse(layer);
				break;
			case NvConv:
				//covolution
				layerConvlutionParse(layer, pre_layer);
				break;
			case 2:
				//sdp
				layerSdpParse(layer, pre_layer);
				break;
			case 3:
				//pdp
				layerPdpParse(layer, pre_layer);
				break;
			case 4:
				layerSoftmaxParse(layer, pre_layer);
				//softmax
				
				break;
			default:
				printf("%s, %d, layer->nvdla_type = %d, error!\n", __FUNCTION__,__LINE__, layer->nvdla_type);
		}
	} 
        debug_info("%s, %d\n", __FUNCTION__, __LINE__);
	//alloc mem for task
	if(mTaskListParser == NULL){
		printf("%s, %d, mTaskListParser is NULL, error!\n", __FUNCTION__, __LINE__);
		return ;
	}
	task_type_list = mTaskListParser->get_tasks_type();
	for(ILoadable::Interface idx : task_type_list){
		taskTypeParse(idx);
	}
	return ;
}

void MemoryListParser::debugMemList(void){
	
	if(0 == mList.size()){
		printf("%s, %d, mList is empty!\n", __FUNCTION__, __LINE__);
		return ;
	}
	debug_info("\n");
	debug_info("------------------mem Entry List info--------------------------\n");
	for(ILoadable::MemoryListEntry mle : mList){
		debug_info("mem_id = %d, size = %lld, flags = %d, domain = %d\n", mle.id, mle.size, mle.flags, mle.domain);
		debug_info("alignment = %d, mle.bind_id = %d, tensor_desc_id = %d\n", mle.alignment, mle.bind_id, mle.tensor_desc_id);
		for(string contenstr : mle.contents){
			debug_info("contents = %s\n", contenstr.c_str());
		}
		for(uint64_t offset : mle.offsets){
			debug_info("offset = %lld\n", offset);
		}
		debug_info("\n");
	}
	return ;
}


} /* namespace nvdla */
