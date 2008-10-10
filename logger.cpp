#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include "buffer.h"

void clunk::log_debug(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

#define FORMAT_BUFFER_SIZE 1024

const std::string clunk::format_string(const char *fmt, ...) {
	va_list ap;

	//try static buffer on the stack to avoid malloc calls
	char static_buf[FORMAT_BUFFER_SIZE];

    va_start(ap, fmt);    
   	int r = vsnprintf (static_buf, FORMAT_BUFFER_SIZE - 1, fmt, ap);
    va_end(ap);

    if (r > -1 && r <= FORMAT_BUFFER_SIZE) 
   		return std::string(static_buf, r);

	int size = FORMAT_BUFFER_SIZE * 2;

	clunk::Buffer buf;

    while(true) {
		buf.set_size(size);
	    va_start(ap, fmt);    
    	int r = vsnprintf ((char *)buf.get_ptr(), size - 1, fmt, ap);
	    va_end(ap);
	    if (r > -1 && r <= size) 
    		return std::string((char *)buf.get_ptr(), r);
    	size *= 2;
    }
}
