#include "net_parser.h"
#include "debug.h"

namespace nvdla{

NetParser::NetParser()
{
    caffe_net = Net();
}
NetParser::~NetParser()
{
}


void NetParser::load_caffe_net(const char * protopath,const char * modelpath)
{

    caffe_net.load_param(protopath);
    caffe_net.load_model(modelpath);

}

void NetParser::build_nvdla_net(void)
{
    std::vector<Layer*>::iterator it;
    Layer * layer;
    for(it = caffe_net.layers.begin(); it != caffe_net.layers.end(); it++)
    {
        layer = *it;
        layer->convert_to_nvdla_layer(&nvdla_layers);
    }

#if 1
    //print the nvdla_layer info
    for(it=nvdla_layers.begin();it != nvdla_layers.end(); it++)
    {
        layer = *it;
        layer->print_layer_info();
        union dla_layer_param_container params = layer->get_params();
        if(layer->nvdla_type == NvPDP){
            debug_info("global_pooling=%d,kernel_h=%d,kernel_w=%d,pad_bottom=%d \n",\
            params.pdp_params.global_pooling,
            params.pdp_params.kernel_h,params.pdp_params.kernel_w,params.pdp_params.pad_bottom
            );

        }
        if(layer->nvdla_type == NvSDP){
            debug_info("action=%d \n",layer->get_action());
        }
    }


    
#endif
}


}






