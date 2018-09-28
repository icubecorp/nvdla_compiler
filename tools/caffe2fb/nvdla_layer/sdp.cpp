
#include "sdp.h"
#include "debug.h"
#include "mat.h"
namespace nvdla {

DEFINE_LAYER_CREATOR(NvdlaSDP)

NvdlaSDP::NvdlaSDP()
{
    src_mem_flag = -1;
    weight_mem_flag = -1;
    dst_mem_flag = -1;
    nvdla_type = NvSDP;
    action = ACTION_NONE;
    set_bpe(2);
}
NvdlaSDP::~NvdlaSDP()
{
}

void NvdlaSDP::set_weight_data(Mat weight_data_p)
{
    weight_data = weight_data_p;
}




void NvdlaSDP::print_layer_info(void)
{

    debug_info("sdp info.........\n");
    debug_info("para...\n");
    debug_info("layer_type=%d,slope=%d \n",nvdla_type,slope);
    debug_info("src_mem_flag=%d,weight_mem_flag=%d,dst_mem_flag=%d\n",src_mem_flag,weight_mem_flag,dst_mem_flag);
    #if 0
    debug_info("weight data top 10...\n");
    float *data = (float *)weight_data.data;
    for(int i =0; i < 10; i++)
    {
        debug_info("index=%d,data=%f\n",i,data++);
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

union dla_layer_param_container NvdlaSDP::get_params(void)
{

    union dla_layer_param_container params;
    params.sdp_params.slope = slope;
    params.sdp_params.weight_data = weight_data;
    return params;
}

void NvdlaSDP::set_action(dla_action action_p)
{
    action = action_p;
}

dla_action NvdlaSDP::get_action(void)
{
    return action;
}

union dla_surface_container NvdlaSDP::fill_dla_surface_des(void)
{
    union dla_surface_container dla_surface_desc;
    memset(&dla_surface_desc, 0, sizeof(union dla_surface_container));
    dla_surface_desc.sdp_surface.dst_data = surface_desc.dst_data;
    dla_surface_desc.sdp_surface.src_data = surface_desc.src_data;
    dla_surface_desc.sdp_surface.x1_data = surface_desc.weight_data;
    return dla_surface_desc;
}

union dla_operation_container NvdlaSDP::fill_dla_op_des(void)
{
    union dla_operation_container dla_op_desc;
    memset(&dla_op_desc, 0, sizeof(union dla_operation_container));
    dla_op_desc.sdp_op.src_precision = PRECISION_FP16;
    dla_op_desc.sdp_op.dst_precision = PRECISION_FP16;
    dla_op_desc.sdp_op.lut_index = -1;
    dla_op_desc.sdp_op.out_cvt.scale = 1;
    dla_op_desc.sdp_op.out_cvt.enable = 1;
    dla_op_desc.sdp_op.conv_mode = CONV_MODE_DIRECT;
    dla_op_desc.sdp_op.batch_num = 1;
    
    dla_op_desc.sdp_op.x1_op.precision = PRECISION_FP16;
    dla_op_desc.sdp_op.x1_op.mul_operand = 1;
    switch(action){
        
        case SDP_ACTION_ADD_BIAS:
            dla_op_desc.sdp_op.x1_op.enable = 1;
            dla_op_desc.sdp_op.x1_op.alu_type = SDP_ALU_OP_SUM;
            dla_op_desc.sdp_op.x1_op.type = SDP_OP_ADD;
            dla_op_desc.sdp_op.x1_op.mode = SDP_OP_PER_KERNEL;
            dla_op_desc.sdp_op.x1_op.act = ACTIVATION_NONE;
            break;
        case SDP_ACTION_RELU:
            dla_op_desc.sdp_op.x1_op.enable = 1;
            dla_op_desc.sdp_op.x1_op.alu_type = SDP_ALU_OP_SUM; //?? 
            dla_op_desc.sdp_op.x1_op.type = SDP_OP_NONE;
            dla_op_desc.sdp_op.x1_op.mode = SDP_OP_PER_LAYER;
            dla_op_desc.sdp_op.x1_op.act = ACTIVATION_RELU;
            break;
        default:
            log_error("not such action %s line=%d\n",__FUNCTION__,__LINE__);
            break;
    }    
    return dla_op_desc;
}



} 





