#include "net.h"

int main()
{
    nvdla::Net lenet;

    lenet.load_param("lenet.param");
    lenet.load_model("lenet.bin");	
    return 0;
}
