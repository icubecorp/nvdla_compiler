
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
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





}
