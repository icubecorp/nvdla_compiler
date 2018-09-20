
#include "nv_softmax.h"
#include "layer_type.h"
#include "debug.h"

namespace nvdla {

DEFINE_LAYER_CREATOR(NvdlaSoftmax)

NvdlaSoftmax::NvdlaSoftmax()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type = NvSoftmax;
}

NvdlaSoftmax::~NvdlaSoftmax()
{
}


void NvdlaSoftmax::fill_params(std::vector<int> params)
{
    std::vector<int>::iterator it = params.begin();
    axis = *it;
}

void NvdlaSoftmax::print_layer_info(void)
{

    debug_info("NvdlaSoftmax info.........\n");
    debug_info("para...\n");
    debug_info("layer_type=%d,axis=%d\n",nvdla_type,axis);
    debug_info("src_mem_flag=%d,weight_mem_flag=%d,dst_mem_flag=%d\n",src_mem_flag,weight_mem_flag,dst_mem_flag);

}

}







