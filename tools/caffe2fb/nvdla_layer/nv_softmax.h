
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
public:
    int axis;

};

} 

#endif


