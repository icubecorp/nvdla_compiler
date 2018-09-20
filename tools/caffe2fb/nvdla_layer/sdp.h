
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
    virtual union dla_layer_param_container get_params(void);
    virtual void set_action(dla_action action_p);
    virtual dla_action get_action(void);

public:
    Mat weight_data;
    float slope;
private:
    dla_action action;
    
};

}

#endif





