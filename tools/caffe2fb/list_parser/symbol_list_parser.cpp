/*
 * symbol_list_parser.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: jiqianxiang
 */

#include "symbol_list_parser.h"
#include "debug.h"
#include <stdlib.h>
#include "nvdla/ILoadable.h"

namespace nvdla {

SymbolListParser::SymbolListParser(NetParser* net, MemoryListParser* memory_parser, TaskListParser* task_parser) :
    ListEntryParser(net),
    mMemoryListParserPtr(memory_parser),
    mTaskListParserPtr(task_parser)
{

}

SymbolListParser::~SymbolListParser() {

}

const void* SymbolListParser::getList() const {
    return (const void*)&mList;
}


int SymbolListParser::get_offset(uint16_t k, uint16_t c, uint16_t h, uint16_t w, struct nvdla_meta_data meta_data)
{
    int offset = -1;
    uint16_t group_num_sum;
    uint16_t group_num_cur;
    uint16_t group_kernel_num_cur;
    int group_size;
    
    uint16_t atomic_num_sum;
    uint16_t atomic_num_cur;
    uint16_t atomic_size_cur;
    //uint16_t atomic_size_reminder;
    switch (meta_data.data_format){
        case WEIGHT_CAFFEMODE:
            offset = k * meta_data.channel * meta_data.height* meta_data.width + \
                     c * meta_data.height * meta_data.width + \
                     h * meta_data.width + w;
            break;
        //for direct conv mode, the memeory data sequence atomic_c -> k -> w -> h -> atomic_num -> group
        case WEIGHT_DIRECT_CONV:
             atomic_size_cur = ATOMIC_C_SIZE;
             atomic_num_sum = roundUp(meta_data.channel * meta_data.bpe, ATOMIC_C_SIZE) /  ATOMIC_C_SIZE;
             atomic_num_cur = roundUp((c + 1) * meta_data.bpe, ATOMIC_C_SIZE) /  ATOMIC_C_SIZE;
             //atomic_size_reminder = ((meta_data.channel * meta_data.bpe)% ATOMIC_C_SIZE);
             //atomic_size_reminder = atomic_size_reminder == 0 ? ATOMIC_C_SIZE : atomic_size_reminder;
             if(atomic_num_cur == atomic_num_sum){
                atomic_size_cur = ((meta_data.channel * meta_data.bpe)% ATOMIC_C_SIZE);
                atomic_size_cur = atomic_size_cur == 0 ? ATOMIC_C_SIZE : atomic_size_cur;
             }
             else
                atomic_size_cur = ATOMIC_C_SIZE;

             group_num_sum = roundUp(meta_data.kernel_num, GROUP_KERNEL_NUM) /  GROUP_KERNEL_NUM;
             group_num_cur = roundUp(k + 1, GROUP_KERNEL_NUM) /  GROUP_KERNEL_NUM;
             //group_size = (atomic_num_sum - 1) * meta_data.height* meta_data.width * ATOMIC_C_SIZE * GROUP_KERNEL_NUM + 
             //               meta_data.height* meta_data.width * atomic_size_reminder * GROUP_KERNEL_NUM;
             group_size =  meta_data.height * meta_data.width * meta_data.channel * meta_data.bpe * GROUP_KERNEL_NUM;
             
             if(group_num_cur == group_num_sum){
                group_kernel_num_cur = (meta_data.kernel_num % GROUP_KERNEL_NUM);
                group_kernel_num_cur = group_kernel_num_cur == 0 ? GROUP_KERNEL_NUM : group_kernel_num_cur;
             }
             else
                group_kernel_num_cur = GROUP_KERNEL_NUM;
             
             offset =   (group_num_cur - 1) * group_size + \
                        (atomic_num_cur - 1) * meta_data.height* meta_data.width * group_kernel_num_cur * ATOMIC_C_SIZE + \
                        h * meta_data.width * group_kernel_num_cur * atomic_size_cur + \
                        w * group_kernel_num_cur *  atomic_size_cur + \
                        (k - GROUP_KERNEL_NUM *(group_num_cur - 1))* atomic_size_cur + (c * meta_data.bpe) % ATOMIC_C_SIZE;
            #if 0
            debug_info("atomic_size_cur=%d,atomic_num_sum=%d,atomic_num_cur=%d,kernel_num=%d, \
                       k=%d,c=%d,h=%d,w=%d,offset=%d  group_num_sum=%d,group_num_cur=%d,group_kernel_num_cur=%d,group_size=%d\n",atomic_size_cur,\
                    atomic_num_sum,atomic_num_cur,meta_data.kernel_num,k,c,h,w,offset,group_num_sum,group_num_cur,group_kernel_num_cur,group_size); 
            #endif
            break;
        case WEIGHT_BIAS:
            offset = (c * meta_data.width * meta_data.height *meta_data.bpe  + h * meta_data.width + w) ;
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
                woffset = get_offset(0, c, h, w, meta_data);
                meta_data.data_format = WEIGHT_BIAS;
                doffset =  get_offset(0, c, h, w, meta_data);
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
      //debug_info("woffset=%d,doffset=%d\n",woffset, doffset);
      return data;
}



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
    uint16_t channel = surface_desc.weight_data.channel;
    int doffset = 0;
    int woffset = 0;
    struct nvdla_meta_data meta_data;
    debug_info("kernel_num=%d, channel=%d,width=%d,height=%d\n",kernel_num,channel,width,height);
    for(uint16_t k = 0; k < kernel_num; k++)
        for(uint16_t c = 0; c < channel; c++)
            for(uint16_t h =0 ; h < height; h++)
                for(uint16_t w = 0; w < width; w++){
                    meta_data.channel = channel;
                    meta_data.width = width;
                    meta_data.height = height;
                    meta_data.kernel_num = kernel_num;
                    meta_data.bpe = bpe;
                    meta_data.data_format = WEIGHT_DIRECT_CONV;
                    doffset= get_offset(k, c, h, w, meta_data);
                    meta_data.data_format = WEIGHT_CAFFEMODE;
                    woffset = get_offset(k, c, h, w, meta_data);
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
                    //debug_info("woffset=%d,doffset=%d w_data=%f m_data=0x%x\n",woffset, doffset,*inp,*(unsigned short*) outp); 
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
        if( layer->weight_mem_flag == 1 ){
            surface_desc = layer->surface_desc;
            mem_id = surface_desc.weight_data.address;
            mem_list_entry = (*mem_list)[mem_id];
            debug_info("enter %s line=%d\n",__FUNCTION__,__LINE__);
            blob.name = mem_list_entry.contents[0];
            blob.size = mem_list_entry.size;
            debug_info("enter %s line=%d\n",__FUNCTION__,__LINE__);
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

void SymbolListParser::fill_emu_taskinfo_blobs(ILoadable::TaskListEntry task_entry){

}

void SymbolListParser::fill_nvdla_taskinfo_blobs(ILoadable::TaskListEntry task_entry){

    std::vector<Layer*> layers = mNetParserPtr->getLayers();
    const std::vector<ILoadable::MemoryListEntry> *mem_list =  \
            (const std::vector<ILoadable::MemoryListEntry> *)mMemoryListParserPtr->getList();
    priv::Loadable::Symbol task_net_desc_blob;
    priv::Loadable::Symbol task_dep_graph_blob;
    priv::Loadable::Symbol task_op_list_blob;
    priv::Loadable::Symbol task_surf_desc_blob;
    Layer * layer;
    NvU16 first_layer_index = task_entry.preactions[0];    
    NvU16 last_layer_index = task_entry.postactions[0];   
    NvU16 task_net_desc_address_index = 0;
    NvU16 task_dep_graph_address_index = task_entry.address_list.size() - STRUCTS_PER_TASK + 1;
    NvU16 task_op_list_address_index = task_entry.address_list.size() - STRUCTS_PER_TASK + 2;
    NvU16 task_surf_desc_address_index = task_entry.address_list.size() - STRUCTS_PER_TASK + 3;
    ILoadable::MemoryListEntry mem_entry;
    
    mem_entry = (*mem_list)[task_entry.address_list[task_op_list_address_index]];
    task_op_list_blob.data = (NvU8 *)malloc(mem_entry.size);
    task_op_list_blob.name = mem_entry.contents[0];
    task_op_list_blob.size = mem_entry.size;
    
    mem_entry = (*mem_list)[task_entry.address_list[task_surf_desc_address_index]];
    task_surf_desc_blob.data = (NvU8 *)malloc(mem_entry.size);
    task_surf_desc_blob.name = mem_entry.contents[0];
    task_surf_desc_blob.size = mem_entry.size;
    
    union dla_operation_container op_desc;
    union dla_surface_container surface_desc;
    NvU8 * op_data = task_op_list_blob.data;
    NvU8 * surface_data = task_surf_desc_blob.data;
    for(NvU16 i = first_layer_index; i <= last_layer_index; i++){
        layer = layers[i];
        op_desc = layer->fill_dla_op_des();
        surface_desc = layer->fill_dla_surface_des();
        memcpy(op_data, &op_desc, sizeof(union dla_operation_container));
        op_data = op_data + sizeof(union dla_operation_container);
        memcpy(surface_data, &surface_desc, sizeof(union dla_surface_container));
        surface_data = surface_data + sizeof(union dla_surface_container);
        
    }
    mList.push_back(task_op_list_blob);
    mList.push_back(task_surf_desc_blob);

}



void SymbolListParser::fill_taskinfo_blobs(void){
    std::vector<ILoadable::TaskListEntry> *task_list = (std::vector<ILoadable::TaskListEntry> *)mTaskListParserPtr->getList();
    ILoadable::TaskListEntry task_entry;
    for(unsigned int i = 0; i < task_list->size(); i++){
        task_entry = (*task_list)[i];
        switch (task_entry.interface){
        case ILoadable::Interface_DLA1:
            fill_nvdla_taskinfo_blobs(task_entry);
            break;
        case ILoadable::Interface_EMU1:
            fill_emu_taskinfo_blobs(task_entry);
            break;
        default:
            printf("error not such task type=%d for blobs\n",task_entry.interface);
            break;
        }
    }
}


void SymbolListParser::buildList() {

    if (!mList.empty()) {
        printf("Warning: list only build for once\n");
        return;
    }
    fill_weight_blobs(&mList, mNetParserPtr, mMemoryListParserPtr); 
    fill_taskinfo_blobs();
}

void SymbolListParser::dump_blobs_info(void){
    priv::Loadable::Symbol symbol;
    for(unsigned int i = 0; i < mList.size(); i++){
        symbol = mList[i];
        NvU8 *data = symbol.data;
        debug_info("name=%s\n",symbol.name.c_str());
        debug_info("start to dump size=%d\n", symbol.size);
        for(unsigned int j = 0; j < symbol.size; j = j+16){
            for(int m = 0; m < 16; m++){
                debug_info("0x%x  ",*data++);
            }
            debug_info("\n");
        }
    }

}

} /* namespace nvdla */
