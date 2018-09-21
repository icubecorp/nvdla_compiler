
#include "layer.h"
#include <stdio.h>
#include <string.h>

namespace nvdla {

Layer::Layer()
{
}

Layer::~Layer()
{
}

int Layer::load_param(const ParamDict& /*pd*/)
{
    return 0;
}

int Layer::load_model(const ModelBin& /*mb*/)
{
    return 0;
}

int Layer::convert_to_nvdla_layer(std::vector<Layer *> *nvdla_layers)
{
    return 0;
}

void Layer::fill_params(std::vector<int> params)
{
}

union dla_layer_param_container Layer::get_params(void)
{
  union dla_layer_param_container params = {};
  return params; 
}


void Layer::set_weight_data(Mat weight_data)
{
}
void Layer::print_layer_info(void)
{
}

void Layer::set_action(dla_action action_p)
{
}

dla_action Layer::get_action(void)
{
    return ACTION_NONE;
}

int Layer::get_bpe(void)
{
    return bpe;
}

void Layer::set_bpe(int bpe_p)
{
    bpe = bpe_p;
}


#include "layer_declaration.h"

static const layer_registry_entry layer_registry[] =
{
#include "layer_registry.h"
};

static const int layer_registry_entry_count = sizeof(layer_registry) / sizeof(layer_registry_entry);

#if NCNN_STRING
int layer_to_index(const char* type)
{
    for (int i=0; i<layer_registry_entry_count; i++)
    {
        if (strcmp(type, layer_registry[i].name) == 0)
            return i;
    }

    return -1;
}

Layer* create_layer(const char* type)
{
    int index = layer_to_index(type);
    if (index == -1)
        return 0;

    return create_layer(index);
}
#endif // NCNN_STRING

Layer* create_layer(int index)
{
    if (index < 0 || index >= layer_registry_entry_count)
        return 0;

    layer_creator_func layer_creator = layer_registry[index].creator;
    if (!layer_creator)
        return 0;

    return layer_creator();
}

}







