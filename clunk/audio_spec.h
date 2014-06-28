#ifndef CLUNK_AUDIO_SPEC_H
#define	CLUNK_AUDIO_SPEC_H

#include <clunk/types.h>
#include <clunk/export_clunk.h>
#include <stdexcept>

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
