
#ifndef LAYER_NV_SOFTMAX_H
#define LAYER_NV_SOFTMAX_H

#include "layer.h"
#include <vector>
namespace nvdla {


class NvdlaSoftmax : public Layer
{
public:
    NvdlaSoftmax();
    ~NvdlaSoftmax();
    virtual void fill_params(std::vector<int> params);
    virtual void print_layer_info(void);
    virtual union dla_layer_param_container get_params(void);
public:
    int axis;

};

} 

#endif


