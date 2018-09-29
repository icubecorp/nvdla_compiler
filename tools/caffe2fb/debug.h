#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "nvdla_interface.h"
#include "dla_interface.h"

namespace nvdla {

	void debug_info(const char *format, ...);
	void log_debug(const char *format, ... );
	void log_info(const char *format, ... );
	void log_warn(const char *format, ... );
	void log_error(const char *format, ... );
    void debug_info_conv_surface_desc(struct dla_conv_surface_desc *desc, int32_t roi);
    void debug_info_conv_op_desc(struct dla_conv_op_desc *desc, int32_t roi);
    void debug_info_pdp_surface_desc(struct dla_pdp_surface_desc *desc, int32_t roi);
    void debug_info_pdp_op_desc(struct dla_pdp_op_desc *desc, int32_t roi);
    void debug_info_sdp_surface_desc(struct dla_sdp_surface_desc *desc, int32_t roi);
    void debug_info_sdp_op_desc(struct dla_sdp_op_desc *desc, int32_t roi);
    void debug_info_op_desc(struct dla_common_op_desc *desc, int32_t roi);
}


#endif 



