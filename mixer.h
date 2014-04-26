#ifndef CLUNK_MIXER_H
#define CLUNK_MIXER_H

#include "audio_spec.h"

namespace clunk {

	static const int MaxMixVolumeShift = 7;
	static const u8 MaxMixVolume = (1 << MaxMixVolumeShift);

	namespace impl {
		template<typename Format> struct Mixer {

			typedef typename Format::Type			Type;
			typedef typename Format::DoubleType		DoubleType;

			static void mix(void *dst_, const void *src_, size_t size, int volume) {
				Type *dst = static_cast<Type *>(dst_);
				const Type *src = static_cast<const Type *>(src_);
				while(size--)
				{
					DoubleType value = (((DoubleType)*dst << MaxMixVolumeShift) + ((DoubleType)*src++ << MaxMixVolumeShift)) >> (MaxMixVolumeShift + 1);
					*dst++ = value;
				}
			}
		};
	}
	struct Mixer {
		static void mix(AudioSpec::Format format, void *dst, const void *src, size_t size, int volume)
		{
			switch(format)
			{
				case AudioSpec::S8:		impl::Mixer<AudioFormat<AudioSpec::S8> >::mix(dst, src, size, volume); break;
				case AudioSpec::S16:	impl::Mixer<AudioFormat<AudioSpec::S16> >::mix(dst, src, size, volume); break;
				case AudioSpec::U8:		impl::Mixer<AudioFormat<AudioSpec::U8> >::mix(dst, src, size, volume); break;
				case AudioSpec::U16:	impl::Mixer<AudioFormat<AudioSpec::U16> >::mix(dst, src, size, volume); break;
			}
		}
	};
}

#endif
