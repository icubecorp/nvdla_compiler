// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "softmax.h"
#include <float.h>
#include <math.h>
#include <algorithm>
#include "debug.h"
namespace nvdla {

DEFINE_LAYER_CREATOR(Softmax)

Softmax::Softmax()
{
}

int Softmax::load_param(const ParamDict& pd)
{
    axis = pd.get(0, 0);
    static int index = 0;
    debug_info("softmax index=%d para.......\n",index++);
    debug_info("axis=%d\n",axis);
    return 0;
}

int Softmax::convert_to_nvdla_layer(std::vector<Layer *> *nvdla_layers)
{
    Layer * layer = create_layer("NvdlaSoftmax");
    if(!layer)
    {
        printf("create layer NvdlaSoftmax failed\n");
        return -1;
    }
    std :: vector < int > params;
    params.push_back(axis);
    layer->fill_params(params);
    nvdla_layers->push_back(layer);
    return 0;
}


}
