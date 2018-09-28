
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
    set_bpe(2);

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


union dla_layer_param_container NvdlaPDP::get_params(void)
{

    union dla_layer_param_container params;
    params.pdp_params.global_pooling = pooling_type;
    params.pdp_params.kernel_w = kernel_w;
    params.pdp_params.kernel_h = kernel_h;
    params.pdp_params.stride_w = stride_w;
    params.pdp_params.stride_h = stride_h;
    params.pdp_params.pad_left = pad_left;
    params.pdp_params.pad_right = pad_right;
    params.pdp_params.pad_top = pad_top;
    params.pdp_params.pad_bottom = pad_bottom;
    params.pdp_params.global_pooling = global_pooling;
    params.pdp_params.pad_mode = pad_mode;
    return params;
}

union dla_surface_container NvdlaPDP::fill_dla_surface_des(void)
{
    union dla_surface_container dla_surface_desc;
    memset(&dla_surface_desc, 0, sizeof(union dla_surface_container));
    dla_surface_desc.pdp_surface.dst_data = surface_desc.dst_data;
    dla_surface_desc.pdp_surface.src_data = surface_desc.src_data;
    return dla_surface_desc;
}

union dla_operation_container NvdlaPDP::fill_dla_op_des(void)
{
    union dla_operation_container dla_op_desc;
    memset(&dla_op_desc, 0, sizeof(union dla_operation_container));
    dla_op_desc.pdp_op.precision = PRECISION_FP16;
    dla_op_desc.pdp_op.split_num = 1; //??
    switch(pooling_type){
        case PoolMethod_MAX:
            dla_op_desc.pdp_op.pool_mode = POOL_MODE_MAX;
            break;
        case PoolMethod_AVE:
            dla_op_desc.pdp_op.pool_mode = POOL_MODE_AVG;
            break;
        default:
            log_error("no such pooling_type %s line=%d\n",__FUNCTION__,__LINE__);
            break;

    }
    dla_op_desc.pdp_op.pool_width = kernel_w - 1;
    dla_op_desc.pdp_op.pool_height = kernel_h - 1;
    dla_op_desc.pdp_op.stride_x = stride_w;
    dla_op_desc.pdp_op.stride_y = stride_h;
    dla_op_desc.pdp_op.pad_left = pad_left;
    dla_op_desc.pdp_op.pad_right = pad_right;
    dla_op_desc.pdp_op.pad_top = pad_top;
    dla_op_desc.pdp_op.pad_bottom = pad_bottom;
    return dla_op_desc;
}


}
