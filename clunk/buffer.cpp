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

#include <clunk/buffer.h>
#include <clunk/clunk_ex.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

using namespace clunk;

void Buffer::fill(const int b) {
	if (_ptr == NULL) 
		return;
	memset(_ptr, b, _size);
}

const Buffer& Buffer::operator=(const Buffer& c) {
	if (this == &c) 
		return *this; // same object

	if (c._ptr == NULL) {
		free();
		return *this;
	}
	assert(c._size > 0);

	set_size(c.get_size());
	memcpy(_ptr, c._ptr, c._size);
	return *this;
}

void Buffer::set_size(size_t s) {
	if (s == _size)
		return;
	
	if (s == 0) {
		free();
		return;
	}

	void * x = realloc(_ptr, s);
	if (x == NULL) 
		throw_io(("realloc (%p, %u)", _ptr, (unsigned)s));
	_ptr = x;
	_size = s;
}

void Buffer::set_data(const void *p, const size_t s) {
	if (p == NULL || s == 0)
		throw_ex(("calling set_data(%p, %u) is invalid", p, (unsigned)s));

	void *x = realloc(_ptr, s);

	if (x == NULL) 
		throw_io(("realloc (%p, %d)", _ptr, (unsigned)s));
	_ptr = x;
	memcpy(_ptr, p, s);
	_size = s;
}

void Buffer::set_data(void *p, const size_t s, const bool own) {
	if (p == NULL || s == 0) 
		throw_ex(("calling set_data(%p, %u, %s) is invalid", p, (unsigned)s, own?"true":"false"));
	
	if (own) {
		free();
		_ptr = p;
		_size = s;
	} else {
		void *x = realloc(_ptr, s);
		if (x == NULL) 
			throw_io(("realloc(%p, %d)", _ptr, (unsigned)s));
		_ptr = x;
		_size = s;
		memcpy(_ptr, p, s);
	}
}

void Buffer::append(const Buffer &other) {
	size_t s1 = _size, s2 = other.get_size();
	if (s2 == 0)
		return;
	set_size(s1 + s2);
	memcpy(static_cast<char *>(_ptr) + s1, other._ptr, s2);
}

void Buffer::append(const void *data, const size_t data_size) {
	if (data_size == 0)
		return;

	size_t s = _size;
	set_size(s + data_size);
	memcpy(static_cast<char *>(_ptr) + s, data, data_size);
}


void* Buffer::reserve(const int more) {
	set_size(_size + more);
	return _ptr;
}

void Buffer::free() {
	if (_ptr != NULL) {
		::free(_ptr);
		_ptr = NULL;
		_size = 0;
	}
}

const std::string Buffer::dump() const {
	if (_ptr == NULL)
		return "empty memory buffer";
	assert(_ptr != 0);
	
	std::string result = clunk::format_string("-[memory dump]-[size: %u]---", (unsigned)_size);
	size_t n = (_size - 1)/ 16 + 1;
	for(size_t i = 0; i < n; ++i) {
		result += clunk::format_string("\n%06x\t", (unsigned)(i * 16));
		size_t j, m = (_size - i * 16);
		if (m > 16) 
			m = 16;
		
		for(j = 0; j < m; ++j) {
			const u8 *p = static_cast<u8 *>(_ptr) + i*16 + j;
			result += clunk::format_string("%02x ", *p);
			if (j == 7) 
				result += " ";
		}
		for(; j < 16; ++j) {
			if (j == 7) 
				result += " ";
			result += "   ";
		}
		result += "\t\t";

		for(j = 0; j < m; ++j) {
			const u8 *p = static_cast<u8 *>(_ptr) + i*16 + j;
			result += clunk::format_string("%c", (*p>=32 && *p < 127)? *p: '.');
			if (j == 7) 
				result += " ";
		}
		for(; j < m; ++j) {
			if (j == 7) 
				result += " ";
			result += " ";
		}
	}
	return result;
}

void Buffer::pop(size_t n) {
	if (_ptr == NULL || n == 0)
		return;
	
	if (n >= _size) {
		free();
		return;
	}
	
	memmove(_ptr, static_cast<u8 *>(_ptr) + n, _size - n);
	set_size(_size - n);
}

void Buffer::unoptimize(void *data, size_t n) { }
