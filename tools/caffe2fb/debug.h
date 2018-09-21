#ifndef _DEBUG_H_
#define _DEBUG_H_


namespace nvdla {

	void debug_info(const char *format, ...);
	void log_debug(const char *format, ... );
	void log_info(const char *format, ... );
	void log_warn(const char *format, ... );
	void log_error(const char *format, ... );

}


#endif 



