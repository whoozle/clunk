#ifndef CLUNK_AUDIO_SPEC_H
#define	CLUNK_AUDIO_SPEC_H

#include "types.h"

namespace clunk {

struct CLUNKAPI AudioSpec {
	enum Format {
		S8,
		U8,
		S16, //only native endianess supported for now, will add later
		U16
	};
	Format	format;
	int		freq;
	u8		channels;

	AudioSpec(): format(S16), freq(), channels() {}
	AudioSpec(Format format, int freq, u8 channels): format(format), freq(freq), channels(channels) {}
};

}

#endif
