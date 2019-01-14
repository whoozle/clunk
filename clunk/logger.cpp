/*
MIT License

Copyright (c) 2008-2019 Netive Media Group & Vladimir Menshakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <clunk/logger.h>
#include <stdio.h>
#include <stdarg.h>
#include <clunk/buffer.h>

void clunk::log_debug(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}

#define FORMAT_BUFFER_SIZE 1024
#ifdef _WINDOWS
#define vsnprintf _vsnprintf
#endif

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
		buf.resize(size);
	    va_start(ap, fmt);    
    	int r = vsnprintf (static_cast<char *>(buf.get_ptr()), size - 1, fmt, ap);
	    va_end(ap);
	    if (r > -1 && r <= size) 
    		return std::string(static_cast<char *>(buf.get_ptr()), r);
    	size *= 2;
    }
}
