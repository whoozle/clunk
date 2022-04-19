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

#ifndef CLUNK_TYPES_H
#define CLUNK_TYPES_H

#include <stdint.h>

#ifndef _USE_MATH_DEFINES
#	define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <clunk/config.h>

namespace clunk
{
	typedef uint8_t		u8;
	typedef uint16_t	u16;
	typedef uint32_t	u32;

	typedef int8_t		s8;
	typedef int16_t		s16;
	typedef int32_t		s32;
}

#if !(defined(__GNUC__) || defined(__GNUG__) || defined(__attribute__))
#	define __attribute__(p) /* nothing */
#endif

#endif
