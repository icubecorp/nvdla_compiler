
#include "convolution.h"
#include "layer_type.h"
#include "debug.h"
#include <vector>

namespace nvdla {

DEFINE_LAYER_CREATOR(Convolution)

Convolution::Convolution()
{
}

Convolution::~Convolution()
{
}


int Convolution::load_param(const ParamDict& pd)
{
    num_output = pd.get(0, 0);
    kernel_w = pd.get(1, 0);
    kernel_h = pd.get(11, kernel_w);
    dilation_w = pd.get(2, 1);
    dilation_h = pd.get(12, dilation_w);
    stride_w = pd.get(3, 1);
    stride_h = pd.get(13, stride_w);
    pad_w = pd.get(4, 0);
    pad_h = pd.get(14, pad_w);
    bias_term = pd.get(5, 0);
    weight_data_size = pd.get(6, 0);
    int8_scale_term = pd.get(8, 0);
    use_int8_inference = pd.use_int8_inference;

    static int index=0;
    debug_info("convolution index=%d para....................\n",index++);
    debug_info("num_output=%d,kernel_w=%d,kernel_h=%d,dilation_w=%d,dilation_h=%d,stride_w=%d,\
				stride_h=%d,pad_w=%d,pad_h=%d,bias_term=%d,weight_data_size=%d,int8_scale_term=%d \n", \
				num_output,kernel_w,kernel_h,dilation_w,dilation_h,stride_w,stride_h,pad_w,pad_h, \
				bias_term,weight_data_size,int8_scale_term);

    if (int8_scale_term == 0)
        use_int8_inference = false;

    return 0;
}

int Convolution::load_model(const ModelBin& mb)
{
    weight_data = mb.load(weight_data_size, 0);
    static int index = 0;
    debug_info("Convolution index=%d mode data......\n",index++);
    debug_info("weigth_data top 10.....\n");
    float * data = (float *)weight_data.data;
    for(int i = 0; i < 10; i++)
    {
        debug_info("index=%d ,data=%f....\n",i, *data++);
    }
    if (weight_data.empty())
        return -100;

    if (bias_term)
    {
        bias_data = mb.load(num_output, 1);
        if (bias_data.empty())
            return -100;
        debug_info("bias_data top 5.....\n");
        float * data = (float *)bias_data.data;
        for(int i = 0; i < 5; i++)
        {
            debug_info("index=%d ,data=%f....\n",i, *data++);
        }
        
    }
    return 0;
}


int Convolution::convert_to_nvdla_layer(std::vector<Layer *> *nvdla_layers)
{   
    Layer * layer = create_layer("NvdlaConv");
    std::vector <int> paras;
    paras.push_back(num_output);
    paras.push_back(kernel_w);
    paras.push_back(kernel_h);
    paras.push_back(dilation_w);
    paras.push_back(dilation_h);
    paras.push_back(stride_w);
    paras.push_back(stride_h);
    paras.push_back(pad_w);
    paras.push_back(pad_h);
    paras.push_back(bias_term);
    paras.push_back(weight_data_size);
   
    if(!layer)
    {
        printf("create layer NvdlaConv failed\n");
        return -1;
    }
    layer->fill_params(paras);
    layer->set_weight_data(weight_data);
    
    nvdla_layers->push_back(layer);
    layer = create_layer("NvdlaSDP");
    if(!layer)
    {
        printf("create layer NvdlaSDP failed\n");
        return -1;
    }
    if (bias_term == 1)
    {
        layer->set_weight_data(bias_data);
    }
    nvdla_layers->push_back(layer);
    return 0;
}        

}






