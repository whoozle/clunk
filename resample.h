#ifndef CLUNK_RESAMPLE_H
#define CLUNK_RESAMPLE_H

#include "audio_spec.h"
#include "buffer.h"

namespace clunk {
	namespace impl {
		template<typename DstAudioFormat, typename SrcAudioFormat>
		struct Resampler {
			static void resample(AudioSpec dst_format, Buffer &dst, AudioSpec src_format, const Buffer &src)
			{
				//fixme
			}
		};

		template<typename DstAudioFormat>
		struct ResamplerDst {
			static void resample(AudioSpec dst_format, Buffer &dst, AudioSpec src_format, const Buffer &src)
			{
				switch(src_format.format)
				{
					case AudioSpec::S8:		impl::Resampler<DstAudioFormat, AudioFormat<AudioSpec::S8> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::S16:	impl::Resampler<DstAudioFormat, AudioFormat<AudioSpec::S16> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::U8:		impl::Resampler<DstAudioFormat, AudioFormat<AudioSpec::U8> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::U16:	impl::Resampler<DstAudioFormat, AudioFormat<AudioSpec::U16> >::resample(dst_format, dst, src_format, src); break;
				}
			}
		};
	}

	struct Resample {
		static void resample(AudioSpec dst_format, Buffer &dst, AudioSpec src_format, const Buffer &src)
		{
			switch(dst_format.format)
			{
				case AudioSpec::S8:		impl::ResamplerDst<AudioFormat<AudioSpec::S8> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::S16:	impl::ResamplerDst<AudioFormat<AudioSpec::S16> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::U8:		impl::ResamplerDst<AudioFormat<AudioSpec::U8> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::U16:	impl::ResamplerDst<AudioFormat<AudioSpec::U16> >::resample(dst_format, dst, src_format, src); break;
			}
		}
	};
}

#endif
