#include "net_parser.h"

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
    }


    
#endif
}


}






