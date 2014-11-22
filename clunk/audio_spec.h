#ifndef CLUNK_AUDIO_SPEC_H
#define	CLUNK_AUDIO_SPEC_H

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
	typedef s8	Type;
	typedef s16	DoubleType;
};

template<>
struct AudioFormat<AudioSpec::S16> {
	typedef s16	Type;
	typedef s32	DoubleType;
};

template<>
struct AudioFormat<AudioSpec::U8> {
	typedef u8	Type;
	typedef u16	DoubleType;
};

template<>
struct AudioFormat<AudioSpec::U16> {
	typedef u16	Type;
	typedef u32	DoubleType;
};

}

#endif
