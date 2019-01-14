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

	resize(c.get_size());
	memcpy(_ptr, c._ptr, c._size);
	return *this;
}

void Buffer::resize(size_t s) {
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
	resize(s1 + s2);
	memcpy(static_cast<char *>(_ptr) + s1, other._ptr, s2);
}

void Buffer::append(const void *data, const size_t data_size) {
	if (data_size == 0)
		return;

	size_t s = _size;
	resize(s + data_size);
	memcpy(static_cast<char *>(_ptr) + s, data, data_size);
}


void* Buffer::reserve(const int more) {
	resize(_size + more);
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
	resize(_size - n);
}

void Buffer::unoptimize(void *data, size_t n) { }
