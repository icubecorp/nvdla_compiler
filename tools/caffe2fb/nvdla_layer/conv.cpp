
#include "conv.h"
#include "layer_type.h"
#include "debug.h"

namespace nvdla {
static int hard_patch_index = 0;
DEFINE_LAYER_CREATOR(NvdlaConv)

NvdlaConv::NvdlaConv()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type = NvConv;
    conv_mode = CONV_MODE_DIRECT;
    set_bpe(2);
    kernel_w = -1;
    kernel_h = -1;
    dilation_w = 1;
    dilation_h = 1;
    stride_w = 1;
    stride_h = 1;
    pad_w = 0;
    pad_h = 0;
    if(hard_patchs[0].entry_per_slice = 7){
        hard_patchs[0].entry_per_slice = 7;
        hard_patchs[0].skip_weight_rls = 0;
        hard_patchs[0].weight_bank = 1;

        hard_patchs[1].entry_per_slice = 6;
        hard_patchs[1].skip_weight_rls = 0;
        hard_patchs[1].weight_bank = 2;

        hard_patchs[2].entry_per_slice = 4;
        hard_patchs[2].skip_weight_rls = 0;
        hard_patchs[2].weight_bank = 2;

        hard_patchs[3].entry_per_slice = 8;
        hard_patchs[3].skip_weight_rls = 1;
        hard_patchs[3].weight_bank = 1;
    }
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

union dla_surface_container NvdlaConv::fill_dla_surface_des(void)
{
    union dla_surface_container dla_surface_desc;
    memset(&dla_surface_desc, 0, sizeof(union dla_surface_container));
    dla_surface_desc.conv_surface.weight_data = surface_desc.weight_data;
    dla_surface_desc.conv_surface.src_data = surface_desc.src_data;
    dla_surface_desc.conv_surface.dst_data = surface_desc.dst_data;
    return dla_surface_desc;
}


union dla_operation_container NvdlaConv::fill_dla_op_des(void)
{
    union dla_operation_container dla_op_desc;
    hard_patch_index++;
    memset(&dla_op_desc, 0, sizeof(union dla_operation_container));
    dla_op_desc.conv_op.conv_mode = conv_mode;
    dla_op_desc.conv_op.skip_weight_rls = hard_patchs[hard_patch_index - 1].skip_weight_rls;
    dla_op_desc.conv_op.entry_per_slice = hard_patchs[hard_patch_index - 1].entry_per_slice;
    dla_op_desc.conv_op.data_format = FORMAT_FEATURE;
    dla_op_desc.conv_op.fetch_grain = 1;
    dla_op_desc.conv_op.batch = 1;
    dla_op_desc.conv_op.weight_format = WEIGHT_FORMAT_UNCOMPRESSED;
    dla_op_desc.conv_op.data_bank = 1;
    dla_op_desc.conv_op.weight_bank = hard_patchs[hard_patch_index - 1].weight_bank;
    dla_op_desc.conv_op.release = surface_desc.src_data.width; // ??
    dla_op_desc.conv_op.input_width_csc = surface_desc.src_data.width;
    dla_op_desc.conv_op.input_height_csc = surface_desc.src_data.height;
    dla_op_desc.conv_op.input_channel_csc = surface_desc.src_data.channel;
    dla_op_desc.conv_op.kernel_channel_csc = surface_desc.src_data.channel;
    dla_op_desc.conv_op.kernel_width_csc = surface_desc.weight_data.width;
    dla_op_desc.conv_op.kernel_height_csc = surface_desc.weight_data.height;
    dla_op_desc.conv_op.input_width_cmac = surface_desc.dst_data.width;
    dla_op_desc.conv_op.input_height_cmac = surface_desc.dst_data.height;
    dla_op_desc.conv_op.bytes_per_kernel = weight_data_size * get_bpe() / num_output;
    dla_op_desc.conv_op.conv_stride_x = stride_w;
    dla_op_desc.conv_op.conv_stride_y = stride_h;
    dla_op_desc.conv_op.dilation_x = dilation_w;
    dla_op_desc.conv_op.dilation_y = dilation_h;
    dla_op_desc.conv_op.pad_x_left = pad_w;
    dla_op_desc.conv_op.pad_x_right = pad_w;
    dla_op_desc.conv_op.pad_y_bottom = pad_h;
    dla_op_desc.conv_op.pad_y_top = pad_h;
    dla_op_desc.conv_op.in_precision = PRECISION_FP16;//hafl_float
    dla_op_desc.conv_op.out_precision = PRECISION_FP16;
    dla_op_desc.conv_op.out_cvt.scale = 1;//??
    dla_op_desc.conv_op.out_cvt.enable = 1;//??
    return dla_op_desc;

}



}







