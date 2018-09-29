
#ifndef LAYER_CONV_H
#define LAYER_CONV_H

#include "layer.h"
#include <vector>
namespace nvdla {


class NvdlaConv : public Layer
{
public:
    NvdlaConv();
    ~NvdlaConv();
    virtual void fill_params(std::vector<int> params);
    virtual void set_weight_data(Mat weight_data);
    virtual void print_layer_info(void);
    virtual union dla_layer_param_container get_params(void);
    virtual union dla_surface_container fill_dla_surface_des(void);
    virtual union dla_operation_container fill_dla_op_des(void);
public:
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

    // model
    Mat weight_data;

    
//    static int hard_patch_index;
//    Mat bias_data;
private:
    int conv_mode;
    //some hardware registers congfiure hard patch for Lenet test
    //static int hard_patch_index;
    struct hard_patch{
         uint8_t skip_weight_rls;
         uint16_t entry_per_slice;
         uint8_t weight_bank;
    }hard_patchs[4]; // 4 is the conv layer num in Lenet
};

} 

#endif


