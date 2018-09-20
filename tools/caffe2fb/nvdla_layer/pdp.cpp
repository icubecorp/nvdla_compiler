
#include "pdp.h"
#include <float.h>
#include <algorithm>
#include "debug.h"

namespace nvdla {

DEFINE_LAYER_CREATOR(NvdlaPDP)

NvdlaPDP::NvdlaPDP()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type =  NvPDP;

}
NvdlaPDP::~NvdlaPDP()
{
}

void NvdlaPDP::fill_params(std::vector<int> params)
{
    std::vector<int>::iterator it = params.begin();
    pooling_type = *it++;
    kernel_w = *it++;
    kernel_h = *it++;
    stride_w = *it++;
    stride_h = *it++;
    pad_left = *it++;
    pad_right = *it++;
    pad_top = *it++;
    pad_bottom = *it++; 
    global_pooling = *it++;
    pad_mode = *it++;
    
}

void NvdlaPDP::print_layer_info(void)
{

    debug_info("pdp info.........\n");
    debug_info("para...\n");
    debug_info("layer_type=%d,pooling_type=%d,kernel_w=%d,kernel_h=%d, \
        stride_w=%d,stride_h=%d,pad_left=%d,pad_right=%d,pad_top=%d, \
        pad_bottom=%d\n",nvdla_type,pooling_type,kernel_w,kernel_h,stride_w,stride_h,pad_left,pad_right, \
        pad_top,pad_bottom);
    debug_info("src_mem_flag=%d,weight_mem_flag=%d,dst_mem_flag=%d\n",src_mem_flag,weight_mem_flag,dst_mem_flag);
}


}
