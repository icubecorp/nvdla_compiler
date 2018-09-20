
#ifndef LAYER_PDP_H
#define LAYER_PDP_H

#include "layer.h"
#include <vector>

namespace nvdla {

class NvdlaPDP : public Layer
{
public:
    NvdlaPDP();
    ~NvdlaPDP();
    virtual void fill_params(std::vector<int> params);
    virtual void print_layer_info(void);
    virtual union dla_layer_param_container get_params(void);
    enum { PoolMethod_MAX = 0, PoolMethod_AVE = 1 };

public:
    // param
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
}

#endif 





