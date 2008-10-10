#ifndef CLUNK_EXCEPTION_H__
#define CLUNK_EXCEPTION_H__

#include <exception>
#include <string>
#include "logger.h"

namespace clunk {
	class CLUNKAPI Exception : public std::exception {
	public: 
		Exception() throw() {} 
		void add_message(const char *file, int line);
		void add_message(const std::string &msg);
		virtual void add_custom_message() {}
		
		virtual ~Exception() throw() {}
		virtual const char* what() const throw() { return message.c_str(); }
	private: 
		std::string message;
	};
	
	class CLUNKAPI IOException : public Exception {
	public: 
		virtual void add_custom_message();
	};
}

#define throw_generic(ex_cl, fmt) { ex_cl e; e.add_message(__FILE__, __LINE__); e.add_message(clunk::format_string fmt ); e.add_custom_message(); throw e; }
#define throw_ex(fmt) throw_generic(clunk::Exception, fmt)
#define throw_io(fmt) throw_generic(clunk::IOException, fmt)

#define TRY try
#define CATCH(where, code) catch(const std::exception &_e) {\
		LOG_ERROR(("%s: %s", where, _e.what())); \
		code;\
	} 

#endif

