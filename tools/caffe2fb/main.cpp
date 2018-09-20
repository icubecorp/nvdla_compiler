#include "net.h"
#include "net_parser.h"
int main()
{
    nvdla::NetParser lenet;

    lenet.load_caffe_net("lenet.param","lenet.bin");
    lenet.build_nvdla_net();
    
    return 0;
}
