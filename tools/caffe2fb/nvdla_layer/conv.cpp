
#include "conv.h"
#include "layer_type.h"
#include "debug.h"

namespace nvdla {

DEFINE_LAYER_CREATOR(NvdlaConv)

NvdlaConv::NvdlaConv()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type = NvConv;
    set_bpe(2);
}

NvdlaConv::~NvdlaConv()
{
}


void NvdlaConv::fill_params(std::vector<int> params)
{
    std::vector<int>::iterator it = params.begin();
    num_output = *it++;
    kernel_w = *it++;
    kernel_h = *it++;
    dilation_w = *it++;
    dilation_h = *it++;
    stride_w = *it++;
    stride_h = *it++;
    pad_w = *it++;
    pad_h = *it++; 
    bias_term = *it++;
    weight_data_size = *it++;
    
}

void  NvdlaConv::set_weight_data(Mat weight_data_p)
{
    weight_data = weight_data_p;
}


void NvdlaConv::print_layer_info(void)
{

    debug_info("conv info.........\n");
    debug_info("para...\n");
    debug_info("layer_type=%d,num_output=%d,kernel_w=%d,kernel_h=%d,dilation_w=%d,dilation_h=%d, \
        stride_w=%d,stride_h=%d,pad_w=%d,pad_h=%d,bias_term=%d, \
        weight_data_size=%d \n",nvdla_type,num_output,kernel_w,kernel_h,dilation_w,dilation_h,stride_w,stride_h,pad_w,pad_h, \
        bias_term,weight_data_size);
    debug_info("src_mem_flag=%d,weight_mem_flag=%d,dst_mem_flag=%d\n",src_mem_flag,weight_mem_flag,dst_mem_flag);
    #if 1
    debug_info("weight data top 10...\n");
    float *data = (float *)weight_data.data;
    for(int i =0; i < 10; i++)
    {
        debug_info("index=%d,data=%f\n",i,*data++);
    }
    #endif 

    #if 0
    debug_info("weight data tail 10...\n");
    data = (float *)weight_data.data;
    for(int i=weight_data_size-10; i< weight_data_size; i++)
    {
        debug_info("index=%d,data=%f\n",i,data[i]);
    }
    #endif
    

}

union dla_layer_param_container NvdlaConv::get_params(void)
{

    union dla_layer_param_container params;
    params.nv_conv_params.bias_term = bias_term;
    params.nv_conv_params.dilation_h = dilation_h;
    params.nv_conv_params.dilation_w = dilation_w;
    params.nv_conv_params.kernel_h = kernel_h;
    params.nv_conv_params.kernel_w = kernel_w;
    params.nv_conv_params.num_output = num_output;
    params.nv_conv_params.pad_h = pad_h;
    params.nv_conv_params.pad_w = pad_w;
    params.nv_conv_params.stride_h = stride_h;
    params.nv_conv_params.stride_w = stride_w;
    params.nv_conv_params.weight_data_size = weight_data_size;
    params.nv_conv_params.weight_data = weight_data.data;
    return params;
}


}







