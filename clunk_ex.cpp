#include "clunk_ex.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#if defined _WINDOWS
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

#ifdef _WINDOWS
	strncpy(buf, _strerror(NULL), sizeof(buf));
#else 
	strncpy(buf, strerror(errno), sizeof(buf));
//	if (strerror_r(errno, buf, sizeof(buf)-1) != 0) perror("strerror");
#endif
	add_message(buf);
}

