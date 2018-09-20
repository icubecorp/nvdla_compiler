#ifndef DLA_INTERFACE_H
#define DLA_INTERFACE_H

enum dla_action{ ACTION_NONE = 0, SDP_ACTION_ADD_BIAS = 1, SDP_ACTION_RELU = 2,};    
struct dla_nv_conv_params
{
    // param
    int num_output;
    int kernel_w;
    int kernel_h;
    int dilation_w;
    int dilation_h;
    int stride_w;
    int stride_h;
    int pad_w;
    int pad_h;
    int bias_term;
    int weight_data_size;
};

struct dla_nv_input_params
{
    int w;
    int h;
    int c;
};

struct dla_sdp_params
{
    float slope;
};


struct dla_pdp_params
{
    int pooling_type;
    int kernel_w;
    int kernel_h;
    int stride_w;
    int stride_h;
    int pad_left;
    int pad_right;
    int pad_top;
    int pad_bottom;
    int global_pooling;
    int pad_mode;
};

struct dla_nv_softmax_params
{
    int axis;
};

union dla_layer_param_container {
    struct dla_nv_input_params nv_input_params;
    struct dla_nv_conv_params nv_conv_params;
    struct dla_pdp_params pdp_params;
    struct dla_sdp_params sdp_params;
    struct dla_nv_softmax_params nv_softmax_params;
};


#endif
