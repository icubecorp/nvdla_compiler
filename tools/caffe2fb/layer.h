// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at

#ifndef NVDLA_LAYER_H
#define NVDLA_LAYER_H

#include <stdio.h>
#include <string>
#include <vector>
#include "mat.h"
#include "modelbin.h"
#include "paramdict.h"
#include "platform.h"


namespace nvdla {


class Allocator;

enum layer_type
{   
    NvInput = 0,
    NvConv = 1,
    NvSDP = 2,
    NvPDP = 3,
    NvSoftmax = 4,
};

class Option
{
public:
    // default option
    Option();

public:
    // light mode
    // intermediate blob will be recycled when enabled
    // enabled by default
    bool lightmode;

    // thread count
    // default value is the one returned by get_cpu_count()
    int num_threads;

    // blob memory allocator
    Allocator* blob_allocator;

    // workspace memory allocator
    Allocator* workspace_allocator;
};

// the global default option
const Option& get_default_option();
int set_default_option(const Option& opt);

struct dla_nv_conv_params
{
    // param
    int num_output;
    int kernel_w;
    int kernel_h;
    int dilation_w;
    int dilation_h;
    int stride_w;
    int stride_h;
    int pad_w;
    int pad_h;
    int bias_term;
    int weight_data_size;
};

struct dla_nv_input_params
{
    int w;
    int h;
    int c;
};

struct dla_sdp_params
{
    float slope;
};


struct dla_pdp_params
{
    int pooling_type;
    int kernel_w;
    int kernel_h;
    int stride_w;
    int stride_h;
    int pad_left;
    int pad_right;
    int pad_top;
    int pad_bottom;
    int global_pooling;
    int pad_mode;
};

struct dla_nv_softmax_params
{
    int axis;
};

union dla_layer_param_container {
    struct dla_nv_input_params nv_input_params;
    struct dla_nv_conv_params nv_conv_params;
    struct dla_pdp_params pdp_params;
    struct dla_sdp_params sdp_params;
    struct dla_nv_softmax_params nv_softmax_params;
};


class Layer
{
public:
    // empty
    Layer();
    // virtual destructor
    virtual ~Layer();

    // load layer specific parameter from parsed dict
    // return 0 if success
    virtual int load_param(const ParamDict& pd);

    // load layer specific weight data from model binary
    // return 0 if success
    virtual int load_model(const ModelBin& mb);
    virtual int convert_to_nvdla_layer(std::vector<Layer *> *nvdla_layers);
    virtual void fill_params(std::vector<int> params);
    virtual union dla_layer_param_container get_params(void);
    virtual void set_weight_data(Mat weight_data);
    virtual void print_layer_info(void);
	
    public:
		// one input and one output blob
		bool one_blob_only;
	
		// support inplace inference
		bool support_inplace;
    public:

#if 0
		// implement inference
		// return 0 if success
		virtual int forward(const std::vector<Mat>& bottom_blobs, std::vector<Mat>& top_blobs, const Option& opt = get_default_option()) const;
		virtual int forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt = get_default_option()) const;
		
		// implement inplace inference
		// return 0 if success
		virtual int forward_inplace(std::vector<Mat>& bottom_top_blobs, const Option& opt = get_default_option()) const;
		virtual int forward_inplace(Mat& bottom_top_blob, const Option& opt = get_default_option()) const;

#endif

public:
#if NCNN_STRING
    // layer type name
    std::string type;
    // layer name
    std::string name;
#endif // NCNN_STRING
    // blob index which this layer needs as input
    std::vector<int> bottoms;
    // blob index which this layer produces as output
    std::vector<int> tops;

    int src_mem_flag;
    int weight_mem_flag;
    int dst_mem_flag;
    layer_type nvdla_type;
};



// layer factory function
typedef Layer* (*layer_creator_func)();

struct layer_registry_entry
{
#if NCNN_STRING
    // layer type name
    const char* name;
#endif // NCNN_STRING
    // layer factory entry
    layer_creator_func creator;
};

#if NCNN_STRING
// get layer type from type name
int layer_to_index(const char* type);
// create layer from type name
Layer* create_layer(const char* type);
#endif // NCNN_STRING
// create layer from layer type
Layer* create_layer(int index);

#define DEFINE_LAYER_CREATOR(name) \
    ::nvdla::Layer* name##_layer_creator() { return new name; }

}

#endif





