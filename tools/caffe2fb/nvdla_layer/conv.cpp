
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
    debug_info("num_output=%d,kernel_w=%d,kernel_h=%d,dilation_w=%d,dilation_h=%d, \
        stride_w=%d,stride_h=%d,pad_w=%d,pad_h=%d,bias_term=%d, \
        weight_data_size=%d \n",num_output,kernel_w,kernel_h,dilation_w,dilation_h,stride_w,stride_h,pad_w,pad_h, \
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

}







