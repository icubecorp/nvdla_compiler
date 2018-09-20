
#ifndef LAYER_NV_INPUT_H
#define LAYER_NV_INPUT_H

#include "layer.h"
#include <vector>

namespace nvdla {

class NvdlaInput : public Layer
{
public:
    NvdlaInput();
    ~NvdlaInput();
    virtual void fill_params(std::vector<int> params);
    virtual void print_layer_info(void);
    virtual union dla_layer_param_container get_params(void);

public:
    int w;
    int h;
    int c;
};

} 

#endif


