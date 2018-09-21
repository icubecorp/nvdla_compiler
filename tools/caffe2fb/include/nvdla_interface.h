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

typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

struct dla_data_cube {
	uint16_t type; /* dla_mem_type */
	int16_t address; /* offset to the actual IOVA in task.address_list */

	uint32_t size;

	/* cube dimensions */
	uint16_t width;
	uint16_t height;

	uint16_t channel;
	uint16_t reserved0;

	/* stride information */
	uint32_t line_stride;
	uint32_t surf_stride;

	/* For Rubik only */
	uint32_t plane_stride;
};

struct dla_surface_desc {
	/* Data cube */
	struct dla_data_cube weight_data;
	struct dla_data_cube src_data;
	struct dla_data_cube dst_data;
};



#endif
