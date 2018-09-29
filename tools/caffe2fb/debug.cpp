
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"
namespace nvdla {

#define LOG_ERROR	0
#define LOG_WARN	1
#define LOG_INFO	2
#define LOG_DEBUG   3

#define LOG_LEVEL	LOG_DEBUG

void debug_info(const char *format, ... ) {
	if (LOG_LEVEL >= LOG_DEBUG) {
		va_list ap;
		va_start( ap, format );
		vprintf(format, ap);
		va_end( ap ); 
	}
}

void log_debug(const char *format, ... ) {
	if (LOG_LEVEL >= LOG_DEBUG) {
		va_list ap;
		va_start( ap, format );
		vprintf(format, ap);
		va_end( ap );
	}
}

void log_info(const char *format, ... ) {
	if (LOG_LEVEL >= LOG_INFO) {
		va_list ap;
		va_start( ap, format );
		vprintf(format, ap);
		va_end( ap );
	}
}

void log_warn(const char *format, ... ) {
	if (LOG_LEVEL >= LOG_WARN) {
		va_list ap;
		va_start( ap, format );
		vprintf(format, ap);
		va_end( ap );
	}
}

void log_error(const char *format, ... ) {
	va_list ap;
	va_start( ap, format );
	vprintf(format, ap);
	va_end( ap );
}


static void
debug_info_data_cube(struct dla_data_cube *cube)
{
    debug_info("    type          = %u\n", cube->type);
    debug_info("    address       = %d\n", cube->address);
    debug_info("    width         = %x\n", cube->width);
    debug_info("    height        = %x\n", cube->height);
    debug_info("    channel       = %x\n", cube->channel);
    debug_info("    size          = %u\n", cube->size);
    debug_info("    line_stride   = %u\n", cube->line_stride);
    debug_info("    surf_stride   = %u\n", cube->surf_stride);
    debug_info("    plane_stride  = %u\n", cube->plane_stride);
    debug_info("]");
}
static void
debug_info_converter(struct dla_cvt_param *cvt)
{
    debug_info("[ scale = %d, truncate = %u, enable = %u, offset = %d ]\n",
            cvt->scale, cvt->truncate, cvt->enable, cvt->offset);
}

void
debug_info_conv_surface_desc(struct dla_conv_surface_desc *desc, int32_t roi)
{
    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_conv_surface_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("weight_data         = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->weight_data);
    debug_info("wmb_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->wmb_data);
    debug_info("wgs_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->wgs_data);
    debug_info("src_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->src_data);
    debug_info("dst_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->dst_data);
    debug_info("offset_u            = %lld\n", desc->offset_u);
    debug_info("in_line_uv_stride   = %u\n", desc->in_line_uv_stride);
}

void
debug_info_conv_op_desc(struct dla_conv_op_desc *desc, int32_t roi)
{
    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_conv_op_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("conv_mode          = %u\n", desc->conv_mode);
    debug_info("data_reuse         = %u\n", desc->data_reuse);
    debug_info("weight_reuse       = %u\n", desc->weight_reuse);
    debug_info("skip_data_rls      = %u\n", desc->skip_data_rls);
    debug_info("skip_weight_rls    = %u\n", desc->skip_weight_rls);
    debug_info("entry_per_slice    = %u\n", desc->entry_per_slice);
    debug_info("data_format        = %u\n", desc->data_format);
    debug_info("pixel_mapping      = %u\n", desc->pixel_mapping);
    debug_info("fetch_grain        = %u\n", desc->fetch_grain);
    debug_info("batch              = %u\n", desc->batch);
    debug_info("weight_format      = %u\n", desc->weight_format);
    debug_info("data_bank          = %u\n", desc->data_bank);
    debug_info("weight_bank        = %u\n", desc->weight_bank);
    debug_info("batch_stride       = %u\n", desc->batch_stride);
    debug_info("post_extension     = %u\n", desc->post_extension);
    debug_info("pixel_override     = %u\n", desc->pixel_override);
    debug_info("release            = %u\n", desc->release);
    debug_info("input_width_csc    = %u\n", desc->input_width_csc);
    debug_info("input_height_csc   = %u\n", desc->input_height_csc);
    debug_info("input_channel_csc  = %u\n", desc->input_channel_csc);
    debug_info("kernel_width_csc   = %u\n", desc->kernel_width_csc);
    debug_info("kernel_height_csc  = %u\n", desc->kernel_height_csc);
    debug_info("kernel_channel_csc = %u\n", desc->kernel_channel_csc);
    debug_info("input_width_cmac   = %u\n", desc->input_width_cmac);
    debug_info("input_height_cmac  = %u\n", desc->input_height_cmac);
    debug_info("bytes_per_kernel   = %u\n", desc->bytes_per_kernel);
    debug_info("mean_ry            = %d\n", desc->mean_ry);
    debug_info("mean_gu            = %d\n", desc->mean_gu);
    debug_info("mean_bv            = %d\n", desc->mean_bv);
    debug_info("mean_ax            = %d\n", desc->mean_ax);
    debug_info("mean_format        = %u\n", desc->mean_format);
    debug_info("conv_stride_x      = %u\n", desc->conv_stride_x);
    debug_info("conv_stride_y      = %u\n", desc->conv_stride_y);
    debug_info("pad_x_left         = %u\n", desc->pad_x_left);
    debug_info("pad_x_right        = %u\n", desc->pad_x_right);
    debug_info("pad_y_top          = %u\n", desc->pad_y_top);
    debug_info("pad_y_bottom       = %u\n", desc->pad_y_bottom);
    debug_info("dilation_x         = %u\n", desc->dilation_x);
    debug_info("dilation_y         = %u\n", desc->dilation_y);
    debug_info("pra_truncate       = %u\n", desc->pra_truncate);
    debug_info("in_precision       = %u\n", desc->in_precision);
    debug_info("out_precision      = %u\n", desc->out_precision);
    debug_info("pad_val            = %d\n", desc->pad_val);
    debug_info("in_cvt             =\n");
    debug_info_converter(&desc->in_cvt);
    debug_info("out_cvt            =\n");
    debug_info_converter(&desc->out_cvt);
}

void
debug_info_pdp_surface_desc(struct dla_pdp_surface_desc *desc, int32_t roi)
{
    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_pdp_surface_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("src_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->src_data);
    debug_info("dst_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->dst_data);
}

void
debug_info_pdp_op_desc(struct dla_pdp_op_desc *desc, int32_t roi)
{
    int32_t i;

    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_pdp_op_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("precision               = %u\n", desc->precision);
    debug_info("padding_value           = [\n");
    for (i = 0; i < PDP_PAD_VAL_NUM; i++)
        debug_info(" %d\n", desc->padding_value[i]);
    debug_info("]\n");
    debug_info("split_num               = %u\n", desc->split_num);
    debug_info("partial_in_width_first  = %u\n",
                    desc->partial_in_width_first);
    debug_info("partial_in_width_mid    = %u\n", desc->partial_in_width_mid);
    debug_info("partial_in_width_last   = %u\n", desc->partial_in_width_last);
    debug_info("partial_width_first     = %u\n", desc->partial_width_first);
    debug_info("partial_width_mid       = %u\n", desc->partial_width_mid);
    debug_info("partial_width_last      = %u\n", desc->partial_width_last);
    debug_info("pool_mode               = %u\n", desc->pool_mode);
    debug_info("pool_width              = %u\n", desc->pool_width);
    debug_info("pool_height             = %u\n", desc->pool_height);
    debug_info("stride_x                = %u\n", desc->stride_x);
    debug_info("stride_y                = %u\n", desc->stride_y);
    debug_info("pad_left                = %u\n", desc->pad_left);
    debug_info("pad_right               = %u\n", desc->pad_right);
    debug_info("pad_top                 = %u\n", desc->pad_top);
    debug_info("pad_bottom              = %u\n", desc->pad_bottom);
}





static void
debug_info_sdp_op(struct dla_sdp_op *sdp_op)
{
    debug_info("    enable         = %u\n", sdp_op->enable);
    debug_info("    alu_type       = %u\n", sdp_op->alu_type);
    debug_info("    type           = %u\n", sdp_op->type);
    debug_info("    mode           = %u\n", sdp_op->mode);
    debug_info("    act            = %u\n", sdp_op->act);
    debug_info("    shift_value    = %u\n", sdp_op->shift_value);
    debug_info("    truncate       = %u\n", sdp_op->truncate);
    debug_info("    precision      = %u\n", sdp_op->precision);
    debug_info("    alu_operand    = %d\n", sdp_op->alu_operand);
    debug_info("    mul_operand    = %d\n", sdp_op->mul_operand);
    debug_info("cvt.alu_cvt          =\n");
    debug_info_converter(&sdp_op->cvt.alu_cvt);
    debug_info("cvt.mul_cvt          =\n");
    debug_info_converter(&sdp_op->cvt.mul_cvt);
    debug_info("]\n");
}

void
debug_info_sdp_surface_desc(struct dla_sdp_surface_desc *desc, int32_t roi)
{
    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_sdp_surface_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("src_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->src_data);
    debug_info("x1_data             = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->x1_data);
    debug_info("x2_data             = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->x2_data);
    debug_info("y_data              = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->y_data);
    debug_info("dst_data            = [ dla_data_cube =>\n");
    debug_info_data_cube(&desc->dst_data);
}

void
debug_info_sdp_op_desc(struct dla_sdp_op_desc *desc, int32_t roi)
{
    debug_info("*********************************************************\n");
    debug_info("NVDLA FW ROI[%d]: dla_sdp_op_desc\n", roi);
    debug_info("---------------------------------------------------------\n");
    debug_info("src_precision    = %u\n", desc->src_precision);
    debug_info("dst_precision    = %u\n", desc->dst_precision);
    debug_info("lut_index        = %d\n", desc->lut_index);
    debug_info("out_cvt          =\n");
    debug_info_converter(&desc->out_cvt);
    debug_info("conv_mode        = %u\n", desc->conv_mode);
    debug_info("batch_num        = %u\n", desc->batch_num);
    debug_info("batch_stride     = %u\n", desc->batch_stride);
    debug_info("x1_op            = [ dla_sdp_op =>\n");
    debug_info_sdp_op(&desc->x1_op);
    debug_info("x2_op            = [ dla_sdp_op =>\n");
    debug_info_sdp_op(&desc->x2_op);
    debug_info("y_op             = [ dla_sdp_op =>\n");
    debug_info_sdp_op(&desc->y_op);
}


void
debug_info_op_desc(struct dla_common_op_desc *desc, int32_t roi)
{
	int32_t i;

	debug_info("*********************************************************\n");
	debug_info("NVDLA FW ROI[%d]: dla_common_op_desc\n", roi);
	debug_info("---------------------------------------------------------\n");
	debug_info("[%p] Operation index %d ROI %d dep_count %d type %d\n",
			(unsigned int *)desc, desc->index, desc->roi_index,
			desc->dependency_count, desc->op_type);
	debug_info("consumers = [ dla_consumer =>\n");
	for (i = 0; i < DLA_OP_NUM; i++)
		debug_info(" [ %d %d ]", desc->consumers[i].index,
					desc->consumers[i].event);
	debug_info("]");
	debug_info("fused_parent = [ dla_consumer =>\n");
	debug_info(" [ %d %d ]", desc->fused_parent.index,
					desc->fused_parent.event);
	debug_info("]");
}


void
debug_info_network_desc(struct dla_network_desc *nd)
{
	debug_info("*********************************************************\n");
	debug_info("NVDLA FW dla_network_desc\n");
	debug_info("---------------------------------------------------------\n");
	debug_info("op desc index      = %d\n", nd->operation_desc_index);
	debug_info("surface desc index = %d\n", nd->surface_desc_index);
	debug_info("dep graph index    = %d\n", nd->dependency_graph_index);
	debug_info("lut data index     = %d\n", nd->lut_data_index);
	debug_info("stat_list_index    = %d\n", nd->stat_list_index);
	debug_info("roi array index    = %d\n", nd->roi_array_index);
	debug_info("surface index      = %d\n", nd->surface_index);
	debug_info("num rois           = %u\n", nd->num_rois);
	debug_info("num ops            = %u\n", nd->num_operations);
	debug_info("num luts           = %u\n", nd->num_luts);
	debug_info("num addr           = %u\n", nd->num_addresses);
	debug_info("input layer        = %u\n", nd->input_layer);
	debug_info("dynamic roi        = %u\n", nd->dynamic_roi);
    for(int i = 0; i < DLA_OP_NUM; i++ ){
        debug_info("nd->op_head[%d]=%d\n",i,nd->op_head[i]);
    }
}


}
