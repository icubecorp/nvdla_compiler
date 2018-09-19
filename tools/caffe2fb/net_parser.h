
#ifndef NVDLA_NET_PARSER_H
#define NVDLA_NET_PARSER_H

#include "net.h"
#include <stdio.h>
#include <vector>


namespace nvdla{


class NetParser{

public:
    NetParser();
    ~NetParser();
    void load_caffe_net(const char * protopath,const char * modelpath);
    void build_nvdla_net(void);

protected:
    const std::vector<Layer*>& getLayers() const {
    	return nvdla_layers;
    }

public:
    Net caffe_net;
    std :: vector < Layer * > nvdla_layers;
    
    
    
        

};




}

#endif





