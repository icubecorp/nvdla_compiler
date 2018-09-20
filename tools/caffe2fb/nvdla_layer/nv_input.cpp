
#include "nv_input.h"
#include "layer_type.h"
#include "debug.h"

namespace nvdla {

DEFINE_LAYER_CREATOR(NvdlaInput)

NvdlaInput::NvdlaInput()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type = NvInput;
}

NvdlaInput::~NvdlaInput()
{
}


void NvdlaInput::fill_params(std::vector<int> params)
{
    std::vector<int>::iterator it = params.begin();
    w = *it++;
    h = *it++;
    c = *it++;
}

void NvdlaInput::print_layer_info(void)
{

    debug_info("input info.........\n");
    debug_info("para...\n");
    debug_info("layer_type=%d,w=%d,h=%d,c=%d\n",nvdla_type,w,h,c);
    debug_info("src_mem_flag=%d,weight_mem_flag=%d,dst_mem_flag=%d\n",src_mem_flag,weight_mem_flag,dst_mem_flag);
}

union dla_layer_param_container NvdlaInput::get_params(void)
{
    union dla_layer_param_container params;
    params.nv_input_params.w = w;
    params.nv_input_params.h = h;
    params.nv_input_params.c = c;
    return params;
}




}







