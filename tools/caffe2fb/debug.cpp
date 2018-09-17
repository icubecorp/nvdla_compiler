
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "debug.h"
namespace nvdla {

#define DEBUG 1
#if DEBUG
	void debug_info(const char *format, ... ) {
		va_list ap;
		va_start( ap, format );
		vprintf(format, ap);
		va_end( ap ); 
	}
#else
	void debug_info(const char *format, ... ) {
	}
#endif 


}
