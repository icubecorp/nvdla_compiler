
#ifndef NCNN_LAYER_TYPE_H
#define NCNN_LAYER_TYPE_H

namespace nvdla {

namespace LayerType {
enum
{
#include "layer_type_enum.h"
    CustomBit = (1<<8),
};
} 

}

#endif // NCNN_LAYER_TYPE_H
