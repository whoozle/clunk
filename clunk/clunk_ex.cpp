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

#include <clunk/clunk_ex.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined _MSC_VER
#    ifndef snprintf
#        define snprintf _snprintf
#    endif
#endif

void clunk::Exception::add_message(const char *file, int line) {
	char buf[1024];
	snprintf(buf, sizeof(buf), "[%s:%d] ", file, line);
	message += buf;
}

void clunk::Exception::add_message(const std::string &msg) {
	message += msg;
	message += ' ';
}

void clunk::IOException::add_custom_message() {
	char buf[1024];
	memset(buf, 0, sizeof(buf));

#ifdef _MSC_VER
	strncpy(buf, _strerror(NULL), sizeof(buf) - 1);
#else 
	strncpy(buf, strerror(errno), sizeof(buf) - 1);
//	if (strerror_r(errno, buf, sizeof(buf)-1) != 0) perror("strerror");
#endif
	add_message(buf);
}

