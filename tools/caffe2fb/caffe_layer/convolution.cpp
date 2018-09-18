
#include "convolution.h"
#include "layer_type.h"
#include "debug.h"

namespace nvdla {

DEFINE_LAYER_CREATOR(Convolution)

Convolution::Convolution()
{
#if 1
   // one_blob_only = true;
   // support_inplace = false;

    quantize = 0;
    dequantize = 0;
#endif
}

Convolution::~Convolution()
{
#if 1
    delete quantize;
    delete dequantize;
#endif
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
    
#if 0

    if (int8_scale_term)
    {
        weight_data_int8_scale = mb.load(1, 1)[0];
        bottom_blob_int8_scale = mb.load(1, 1)[0];
    }

    bool weight_data_is_int8 = (weight_data.elemsize == (size_t)1u);
    bool weight_data_is_float32 = (weight_data.elemsize == (size_t)4u);

    if (weight_data_is_int8 && !use_int8_inference)
    {
        fprintf(stderr, "quantized int8 weight loaded but use_int8_inference disabled\n");
        return -1;
    }

    if (weight_data_is_float32 && use_int8_inference)
    {
        // quantize weight to int8
        Layer* op = nvdla::create_layer(nvdla::LayerType::Quantize);

        nvdla::ParamDict pd;
        pd.set(0, weight_data_int8_scale);// scale

        op->load_param(pd);

        Mat int8_weight_data;
        op->forward(weight_data, int8_weight_data);

        delete op;

        if (int8_weight_data.empty())
            return -100;

        weight_data = int8_weight_data;
    }

    if (use_int8_inference)
    {
        quantize = nvdla::create_layer(nvdla::LayerType::Quantize);
        {
            nvdla::ParamDict pd;
            pd.set(0, bottom_blob_int8_scale);// scale

            quantize->load_param(pd);
        }

        dequantize = nvdla::create_layer(nvdla::LayerType::Dequantize);
        {
            float top_rescale = 1.f / (bottom_blob_int8_scale * weight_data_int8_scale);

            nvdla::ParamDict pd;
            pd.set(0, top_rescale);// scale
            pd.set(1, bias_term);// bias_term
            pd.set(2, num_output);// bias_data_size

            dequantize->load_param(pd);

            nvdla::Mat weights[1];
            weights[0] = bias_data;

            dequantize->load_model(ModelBinFromMatArray(weights));
        }
    }
#endif

    return 0;
}

#if 0

int Convolution::forward(const Mat& bottom_blob, Mat& top_blob, const Option& opt) const
{
    // convolv with NxN kernel
    // value = value + bias

    // flattened blob, implement as InnerProduct
    if (bottom_blob.dims == 1 && kernel_w == 1 && kernel_h == 1)
    {
        int num_input = weight_data_size / num_output;
        if (bottom_blob.w == num_input)
        {
            // call InnerProduct
            nvdla::Layer* op = nvdla::create_layer(nvdla::LayerType::InnerProduct);

            // set param
            nvdla::ParamDict pd;
            pd.set(0, num_output);
            pd.set(1, bias_term);
            pd.set(2, weight_data_size);
            pd.set(8, int8_scale_term);

            pd.use_int8_inference = use_int8_inference;

            op->load_param(pd);

            // set weights
            nvdla::Mat weights[4];
            weights[0] = weight_data;
            weights[1] = bias_data;

            if (int8_scale_term)
            {
                weights[2] = Mat(1, (size_t)4u, (void*)&weight_data_int8_scale);
                weights[3] = Mat(1, (size_t)4u, (void*)&bottom_blob_int8_scale);
            }

            op->load_model(ModelBinFromMatArray(weights));

            // forward
            op->forward(bottom_blob, top_blob, opt);

            delete op;

            return 0;
        }
    }

    int w = bottom_blob.w;
    int h = bottom_blob.h;
    int channels = bottom_blob.c;
    size_t elemsize = bottom_blob.elemsize;

//     fprintf(stderr, "Convolution input %d x %d  pad = %d %d  ksize=%d %d  stride=%d %d\n", w, h, pad_w, pad_h, kernel_w, kernel_h, stride_w, stride_h);

    const int kernel_extent_w = dilation_w * (kernel_w - 1) + 1;
    const int kernel_extent_h = dilation_h * (kernel_h - 1) + 1;

    Mat bottom_blob_unbordered = bottom_blob;
    if (use_int8_inference && elemsize != 1)
    {
        Mat bottom_blob_int8;
        bottom_blob_int8.create(w, h, channels, (size_t)1u, opt.workspace_allocator);
        if (bottom_blob_int8.empty())
            return -100;

        // quantize, scale and round to nearest
        {
            nvdla::Option opt_g = opt;
            opt_g.blob_allocator = bottom_blob_int8.allocator;

            quantize->forward(bottom_blob, bottom_blob_int8, opt_g);
        }

        bottom_blob_unbordered = bottom_blob_int8;
    }

    Mat bottom_blob_bordered = bottom_blob_unbordered;
    if (pad_w > 0 || pad_h > 0)
    {
        copy_make_border(bottom_blob_unbordered, bottom_blob_bordered, pad_h, pad_h, pad_w, pad_w, BORDER_CONSTANT, 0.f, opt.workspace_allocator, opt.num_threads);
        if (bottom_blob_bordered.empty())
            return -100;

        w = bottom_blob_bordered.w;
        h = bottom_blob_bordered.h;
    }
    else if (pad_w == -233 && pad_h == -233)
    {
        int wpad = kernel_extent_w + (w - 1) / stride_w * stride_w - w;
        int hpad = kernel_extent_h + (h - 1) / stride_h * stride_h - h;
        if (wpad > 0 || hpad > 0)
        {
            copy_make_border(bottom_blob_unbordered, bottom_blob_bordered, hpad / 2, hpad - hpad / 2, wpad / 2, wpad - wpad / 2, BORDER_CONSTANT, 0.f, opt.workspace_allocator, opt.num_threads);
            if (bottom_blob_bordered.empty())
                return -100;
        }

        w = bottom_blob_bordered.w;
        h = bottom_blob_bordered.h;
    }

    int outw = (w - kernel_extent_w) / stride_w + 1;
    int outh = (h - kernel_extent_h) / stride_h + 1;

    top_blob.create(outw, outh, num_output, elemsize, opt.blob_allocator);
    if (top_blob.empty())
        return -100;

    const int maxk = kernel_w * kernel_h;

    // kernel offsets
    std::vector<int> _space_ofs(maxk);
    int* space_ofs = &_space_ofs[0];
    {
        int p1 = 0;
        int p2 = 0;
        int gap = w * dilation_h - kernel_w * dilation_w;
        for (int i = 0; i < kernel_h; i++)
        {
            for (int j = 0; j < kernel_w; j++)
            {
                space_ofs[p1] = p2;
                p1++;
                p2 += dilation_w;
            }
            p2 += gap;
        }
    }

    if (use_int8_inference)
    {
        // num_output
        #pragma omp parallel for num_threads(opt.num_threads)
        for (int p=0; p<num_output; p++)
        {
            int* outptr = top_blob.channel(p);

            for (int i = 0; i < outh; i++)
            {
                for (int j = 0; j < outw; j++)
                {
                    int sum = 0;

                    const signed char* kptr = (const signed char*)weight_data + maxk * channels * p;

                    // channels
                    for (int q=0; q<channels; q++)
                    {
                        const Mat m = bottom_blob_bordered.channel(q);
                        const signed char* sptr = m.row<signed char>(i*stride_h) + j*stride_w;

                        for (int k = 0; k < maxk; k++)
                        {
                            int val = sptr[ space_ofs[k] ];
                            int w = kptr[k];
                            sum += val * w;
                        }

                        kptr += maxk;
                    }

                    outptr[j] = sum;
                }

                outptr += outw;
            }
        }

        // dequantize, reverse scale inplace
        {
            nvdla::Option opt_g = opt;
            opt_g.blob_allocator = top_blob.allocator;

            dequantize->forward_inplace(top_blob, opt_g);
        }

        return 0;
    }

    // num_output
    #pragma omp parallel for num_threads(opt.num_threads)
    for (int p=0; p<num_output; p++)
    {
        float* outptr = top_blob.channel(p);

        for (int i = 0; i < outh; i++)
        {
            for (int j = 0; j < outw; j++)
            {
                float sum = 0.f;

                if (bias_term)
                    sum = bias_data[p];

                const float* kptr = (const float*)weight_data + maxk * channels * p;

                // channels
                for (int q=0; q<channels; q++)
                {
                    const Mat m = bottom_blob_bordered.channel(q);
                    const float* sptr = m.row(i*stride_h) + j*stride_w;

                    for (int k = 0; k < maxk; k++) // 29.23
                    {
                        float val = sptr[ space_ofs[k] ]; // 20.72
                        float w = kptr[k];
                        sum += val * w; // 41.45
                    }

                    kptr += maxk;
                }

                outptr[j] = sum;
            }

            outptr += outw;
        }
    }

    return 0;
}

#endif

}






