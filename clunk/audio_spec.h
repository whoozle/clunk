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

#ifndef CLUNK_AUDIO_SPEC_H
#define	CLUNK_AUDIO_SPEC_H

#include <clunk/types.h>
#include <clunk/export_clunk.h>
#include <stdexcept>
#include <limits>

namespace clunk {

struct CLUNKAPI AudioSpec {
	enum Format {
		S8,
		U8,
		S16, //only native endianess supported for now, will add later
		U16
	};
	Format	format;
	int		sample_rate;
	u8		channels;

	unsigned bytes_per_sample() const {
		switch(format)
		{
		case S8:	case U8:	return 1;
		case S16:	case U16:	return 2;
		default:
			throw std::runtime_error("invalid format");
		}
	}

	unsigned max_sample_value() const {
		switch(format)
		{
		case S8:	return std::numeric_limits<s8>::max();
		case U8:	return std::numeric_limits<u8>::max();
		case S16:	return std::numeric_limits<s16>::max();
		case U16:	return std::numeric_limits<u16>::max();
		default:
			throw std::runtime_error("invalid format");
		}
	}

	unsigned zero_sample_value() const {
		switch(format)
		{
		case S8:	return 0;
		case U8:	return (1 + (unsigned)std::numeric_limits<u8>::max()) / 2;
		case S16:	return 0;
		case U16:	return (1 + (unsigned)std::numeric_limits<u16>::max()) / 2;
		default:
			throw std::runtime_error("invalid format");
		}
	}

	bool is_signed() const
	{ return format == S8 || format == S16; }

	bool is_unsigned() const
	{ return !is_signed(); }

	AudioSpec(): format(S16), sample_rate(), channels() {}
	AudioSpec(Format format, int freq, u8 channels): format(format), sample_rate(freq), channels(channels) {}
};

template<int Format>
struct AudioFormat {};

template<>
struct AudioFormat<AudioSpec::S8> {
	typedef s8			Type;
	typedef s16			DoubleType;

	static const Type	Min = -128;
	static const Type	Max = 127;
	static const Type	Zero = 0;
	static const Type	Range = Max - Zero;

	template<typename T>
	static s8 clip(const T & value)
	{ return value >= Min? value <= Max? value: Max: Min; }
};

template<>
struct AudioFormat<AudioSpec::S16> {
	typedef s16	Type;
	typedef s32	DoubleType;

	static const Type	Min = -32768;
	static const Type	Max = 32767;
	static const Type	Zero = 0;
	static const Type	Range = Max - Zero;

	template<typename T>
	static s16 clip(const T & value)
	{ return value >= Min? value <= Max? value: Max: Min; }
};

template<>
struct AudioFormat<AudioSpec::U8> {
	typedef u8	Type;
	typedef u16	DoubleType;

	static const Type	Min = 0;
	static const Type	Max = 255;
	static const Type	Zero = 128;
	static const Type	Range = Max - Zero;

	template<typename T>
	static u8 clip(const T & value)
	{ return value >= Min? value <= Max? value: Max: Min; }
};

template<>
struct AudioFormat<AudioSpec::U16> {
	typedef u16	Type;
	typedef u32	DoubleType;

	static const Type	Min = 0;
	static const Type	Max = 65535;
	static const Type	Zero = 32768;
	static const Type	Range = Max - Zero;

	template<typename T>
	static u16 clip(const T & value)
	{ return value >= Min? value <= Max? value: Max: Min; }
};

}

#endif
