/*
 * symbol_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "symbol_list_parser.h"
#include "debug.h"
#include <stdlib.h>

namespace nvdla {

SymbolListParser::SymbolListParser(NetParser* net, MemoryListParser* memory_parser) :
    ListEntryParser(net),
    mMemoryListParserPtr(memory_parser)
{

}

SymbolListParser::~SymbolListParser() {

}

const void* SymbolListParser::getList() const {
    return (const void*)&mList;
}


int SymbolListParser::get_offset(uint16_t a, uint16_t w, uint16_t h, uint16_t k, 
                                    uint16_t c,  struct nvdla_meta_data meta_data)
{
    int offset = -1;
    switch (meta_data.data_format){
        case WEIGHT_CAFFEMODE:
            offset = k * meta_data.channel * meta_data.height* meta_data.width + \
                        (a * ATOMIC_C_SIZE + c) * meta_data.height * meta_data.width + \
                        h*meta_data.width + w;
                        
            break;
        case WEIGHT_DIRECT_CONV:
            offset = a * meta_data.height* meta_data.width * meta_data.kernel_num * ATOMIC_C_SIZE + \
                        h * meta_data.width * meta_data.kernel_num * meta_data.dynamic_atomic_size + \
                        w * meta_data.kernel_num *  meta_data.dynamic_atomic_size + \
                        k *  meta_data.dynamic_atomic_size + c;
            offset = offset * meta_data.bpe;
            break;
        case WEIGHT_BIAS:
            offset = (c * meta_data.width * meta_data.height + h * meta_data.width + w) * meta_data.bpe;
            break;
        default:
            printf("error not such format =%d \n",meta_data.data_format);
           break;
    }
    return offset;
}


void *SymbolListParser::fill_bias_weight_data(Layer * layer){

    struct dla_surface_desc surface_desc = layer->surface_desc;
    NvU64 mem_size = surface_desc.weight_data.size;
    union dla_layer_param_container params = layer->get_params();
    float * weight_data = (float *)params.sdp_params.weight_data;
    NvU8 *data = (NvU8 *)malloc(mem_size);
    uint16_t bpe = (uint16_t)layer->get_bpe();
    uint16_t width = surface_desc.weight_data.width;
    uint16_t height = surface_desc.weight_data.height;
    uint16_t channel = surface_desc.weight_data.channel;
    int doffset = 0;
    int woffset = 0;
    struct nvdla_meta_data meta_data;
    for(int c = 0; c < channel; c++)
        for(int h = 0; h < height; h++)
            for(int w = 0; w < width; w++){
                
                meta_data.channel = channel;
                meta_data.width = width;
                meta_data.height = height;
                meta_data.bpe = bpe;
                meta_data.data_format = WEIGHT_CAFFEMODE;
                woffset = get_offset(0, w, h, 0, c, meta_data);
                meta_data.data_format = WEIGHT_BIAS;
                doffset =  get_offset(0, w, h, 0, c, meta_data);
                if (doffset < 0){
                    printf("error doffset < 0 \n");
                    return NULL;
                }
                if (woffset < 0){
                    printf("error woffset < 0 \n");
                    return NULL;
                }
                float* inp = weight_data + woffset;
                half_float::half* outp = reinterpret_cast<half_float::half*>(data + doffset);
                *outp = half_float::half(*inp);                

            }
      return data;
}


//for direct conv mode, the memeory data sequence atomic_c -> k -> w -> h -> atomic_num
void *SymbolListParser::fill_conv_weight_data(Layer * layer){

    struct dla_surface_desc surface_desc = layer->surface_desc;
    NvU64 mem_size = surface_desc.weight_data.size;
    union dla_layer_param_container params = layer->get_params();
    float * weight_data = (float *)params.nv_conv_params.weight_data;
    NvU8 *data = (NvU8 *)malloc(mem_size);
    uint16_t bpe = (uint16_t)layer->get_bpe();
    uint16_t width = surface_desc.weight_data.width;
    uint16_t height = surface_desc.weight_data.height;
    uint16_t kernel_num = surface_desc.dst_data.channel; // kernel num
    uint16_t c = surface_desc.weight_data.channel;
    uint16_t atomic_size = ATOMIC_C_SIZE;
    uint16_t atomic_num = roundUp(c * bpe, ATOMIC_C_SIZE) /  ATOMIC_C_SIZE;
    int doffset = 0;
    int woffset = 0;
    struct nvdla_meta_data meta_data;
    for(uint16_t a = 0; a < atomic_num; a++)
        for(uint16_t h =0 ; h < height; h++)
            for(uint16_t w = 0; w < width; w++)
                for(uint16_t k = 0; k < kernel_num; k++)
                {
                    if(atomic_num == (a + 1))
                        atomic_size = c % ATOMIC_C_SIZE;
                    else
                        atomic_size = ATOMIC_C_SIZE;
                    for(uint16_t size = 0; size < atomic_size ;size++)
                    {   
                        meta_data.channel = c;
                        meta_data.width = width;
                        meta_data.height = height;
                        meta_data.kernel_num = kernel_num;
                        meta_data.bpe = bpe;
                        meta_data.data_format = WEIGHT_DIRECT_CONV;
                        meta_data.dynamic_atomic_size = atomic_size;
                        doffset= get_offset(a, h, w, k, size, meta_data);
                        meta_data.data_format = WEIGHT_CAFFEMODE;
                        woffset = get_offset(a, h, w, k, size, meta_data);
                        if (doffset < 0){
                            printf("error doffset < 0 \n");
                            return NULL;
                        }
                        if (woffset < 0){
                            printf("error woffset < 0 \n");
                            return NULL;
                        }
                        float* inp = weight_data + woffset;
                        half_float::half* outp = reinterpret_cast<half_float::half*>(data + doffset);
                        *outp = half_float::half(*inp);
                    }
                        
                }
    return data;
}


void SymbolListParser::fill_weight_blobs(std::vector<priv::Loadable::Symbol> *mlist,
        NetParser* net, MemoryListParser* memory_parser){

    std::vector<Layer*> layers = mNetParserPtr->getLayers();
    const std::vector<ILoadable::MemoryListEntry> *mem_list =  \
            (const std::vector<ILoadable::MemoryListEntry> *)memory_parser->getList();
    priv::Loadable::Symbol blob;
    Layer * layer;
    int mem_id = 0;
    ILoadable::MemoryListEntry mem_list_entry;
    struct dla_surface_desc surface_desc;
    for(unsigned int i = 0; i < layers.size(); i++){
        layer = layers[i];
        if( layer->weight_mem_flag ){
            
            surface_desc = layer->surface_desc;
            mem_id = surface_desc.weight_data.address;
            mem_list_entry = (*mem_list)[mem_id];
            blob.name = mem_list_entry.contents[0];
            blob.size = mem_list_entry.size;
            debug_info("blob name=%s,size=%d,mem_id=%d\n",blob.name.c_str(),blob.size,mem_id);
            switch (layer->nvdla_type){
                case NvConv:
                    blob.data = (NvU8 *)fill_conv_weight_data(layer);
                    break;
                case NvSDP:
                    blob.data = (NvU8 *)fill_bias_weight_data(layer);
                    break;
                default:
                    printf("error: not such layer type = %d for weight\n",layer->nvdla_type);
                    break;
            }

            mlist->push_back(blob);
        }
        
    }

}

void SymbolListParser::buildList() {

    if (!mList.empty()) {
        printf("Warning: list only build for once\n");
        return;
    }
    fill_weight_blobs(&mList, mNetParserPtr, mMemoryListParserPtr);
}

} /* namespace nvdla */
