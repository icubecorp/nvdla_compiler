
#ifndef NCNN_PARAMDICT_H
#define NCNN_PARAMDICT_H

#include <stdio.h>
#include "mat.h"
#include "platform.h"

// at most 20 parameters
#define NCNN_MAX_PARAM_COUNT 20

namespace nvdla {

class Net;
class ParamDict
{
public:
    // empty
    ParamDict();

    // get int
    int get(int id, int def) const;
    // get float
    float get(int id, float def) const;
    // get array
    Mat get(int id, const Mat& def) const;

    // set int
    void set(int id, int i);
    // set float
    void set(int id, float f);
    // set array
    void set(int id, const Mat& v);

public:
    int use_winograd_convolution;
    int use_sgemm_convolution;
    int use_int8_inference;

protected:
    friend class Net;

    void clear();

#if NCNN_STDIO
#if NCNN_STRING
    int load_param(FILE* fp);
#endif // NCNN_STRING
    int load_param_bin(FILE* fp);
#endif // NCNN_STDIO
    int load_param(const unsigned char*& mem);

protected:
    struct
    {
        int loaded;
        union { int i; float f; };
        Mat v;
    } params[NCNN_MAX_PARAM_COUNT];
};
}

#endif
