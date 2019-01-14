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

#ifndef CLUNK_EXCEPTION_H__
#define CLUNK_EXCEPTION_H__

/* clunk - cross-platform 3D audio API built on top SDL library
 * Copyright (C) 2007-2014 Netive Media Group
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/


#include <exception>
#include <string>
#include <clunk/logger.h>

namespace clunk
{

	class CLUNKAPI Exception : public std::exception
	{
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

	class CLUNKAPI IOException : public Exception
	{
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

