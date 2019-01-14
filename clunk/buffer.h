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

#ifndef CLUNK_BUFFER_H__
#define CLUNK_BUFFER_H__

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


#include <clunk/types.h>
#include <clunk/export_clunk.h>
#include <string>

namespace clunk {

	/*!
		\brief Memory buffer
		This class contains single memory buffer, allocated by malloc.
		It auto frees it when it goes out of scope.
	*/

	class CLUNKAPI Buffer
	{
	public:
		//! Default ctor, empty buffer.
		Buffer(): _ptr(nullptr), _size(0) {}
		//! Copy ctor
		Buffer(const Buffer& c) : _ptr(nullptr), _size(0) { *this = c; }
		Buffer(Buffer&& c) : _ptr(c._ptr), _size(c._size) { c._ptr = nullptr; c._size = 0; }
		/*!
				\brief Instantly allocates 'size' memory
				\param[in] size size of the memory buffer
		*/
		Buffer(int size): _ptr(nullptr), _size(0) { resize(size); }

		//! Destructor, deallocates buffer if needed
		~Buffer() { free(); }

		//! Gets pointer to the memory buffer
		void *get_ptr() const { return _ptr; }
		//! Gets pointer to the memory buffer (const)
		size_t get_size() const { return _size; }
		/*!
				\brief Tests if buffer was empty
				\return returns true if the buffer is empty or deallocated.
		*/
		bool empty() const { return _ptr == nullptr; }

		/*!
				\brief Leaks buffer's content. Use it with care.
				Leaks buffer's content.
				Useful for exception-safe passing of malloc'ed memory to some library function which later deallocates it.
		*/
		std::pair<void *, size_t> unlink() { auto r = std::make_pair(_ptr, _size); _ptr = nullptr; _size = 0; return r; }

		//! Default operator=
		const Buffer& operator=(const Buffer& c);

		//! Frees buffer. you do not have to call it exlicitly everywhere!
		void free();
		/*!
				\brief Sets size of the buffer
				reallocates buffer to the given size. May throw exception!
				\param[in] s size of the new buffer.
		*/
		void resize(size_t s);
		void set_size(size_t s) { resize(s); } ///< @deprecated
		/*! \brief Sets buffer content to a given data.
				Copies given data to the buffer. Note, that functions allocates memory for a new buffer. Do not forget to deallocate 'p' if needed.
				\param[in] p source pointer
				\param[in] s size of the data to be copied.
		*/
		void set_data(const void *p, const size_t s);
		/*! \brief Sets buffer content to a given data.
				Copies given data to the buffer. Note, that functions allocates memory for a new buffer if 'own' == false. Do not forget to deallocate 'p' if needed.
				\param[in] p source pointer
				\param[in] s size of the data to be copied.
				\param[in] own grab pointer and deallocate it later automatically.
		*/
		void set_data(void *p, const size_t s, const bool own = false);

		//! Fills contents of the buffer with given byte.
		void fill(int b);

		//! Appends given buffer.
		void append(const Buffer &other);
		//! Appends given buffer.
		void append(const void *data, const size_t size);

		/*!
				\brief Add more bytes to the end of the buffer.
				\param[in] more number of bytes to be allocated.
		*/
		void *reserve(int more);

		//! Returns nice string representation for the buffer. Useful for debugging.
		const std::string dump() const;

		//! Pops n bytes from the front
		void pop(size_t n);

		///no-op to remove optimization of unused result
		static void unoptimize(void *data, size_t n);

	private:
		void *_ptr;
		size_t _size;
	};

}

#endif
