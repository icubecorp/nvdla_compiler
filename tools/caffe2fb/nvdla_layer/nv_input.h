
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
public:
    int w;
    int h;
    int c;
};

} 

#endif


