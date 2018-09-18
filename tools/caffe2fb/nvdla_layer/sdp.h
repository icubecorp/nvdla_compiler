
#ifndef LAYER_SDP_H
#define LAYER_SDP_H

#include "layer.h"

namespace nvdla {

class NvdlaSDP : public Layer
{
public:
    NvdlaSDP();
    ~NvdlaSDP();
    virtual void set_weight_data(Mat weight_data);
    virtual void print_layer_info(void);
public:
    Mat weight_data;
    float slope;
    
};

}

#endif





