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

#ifndef CLUNK_RESAMPLE_H
#define CLUNK_RESAMPLE_H

#include <clunk/audio_spec.h>
#include <clunk/buffer.h>
#include <clunk/logger.h>
#include <stdexcept>

namespace clunk {
	namespace impl {
		template<typename DstType, typename SrcType>
		struct SampleResampler;

		//remove this
		template<> struct SampleResampler<u8, u8>	{ static void Write(u8 &dst, const u8 &src)		{ dst = src; } };
		template<> struct SampleResampler<u8, s8>	{ static void Write(u8 &dst, const s8 &src)		{ dst = src + 128; } };
		template<> struct SampleResampler<u8, u16>	{ static void Write(u8 &dst, const u16 &src)	{ dst = src >> 8; } };
		template<> struct SampleResampler<u8, s16>	{ static void Write(u8 &dst, const s16 &src)	{ dst = (src + (u16)32768) >> 8; } };

		template<> struct SampleResampler<s8, u8>	{ static void Write(s8 &dst, const u8 &src)		{ dst = src - 128; } };
		template<> struct SampleResampler<s8, s8>	{ static void Write(s8 &dst, const s8 &src)		{ dst = src; } };
		template<> struct SampleResampler<s8, u16>	{ static void Write(s8 &dst, const u16 &src)	{ dst = (src - (u16)32768) >> 8; } };
		template<> struct SampleResampler<s8, s16>	{ static void Write(s8 &dst, const s16 &src)	{ dst = src >> 8; } };

		template<> struct SampleResampler<u16, u8>	{ static void Write(u16 &dst, const u8 &src)	{ dst = src << 8; } };
		template<> struct SampleResampler<u16, s8>	{ static void Write(u16 &dst, const s8 &src)	{ dst = (src + (u16)128) << 8; } };
		template<> struct SampleResampler<u16, u16>	{ static void Write(u16 &dst, const u16 &src)	{ dst = src; } };
		template<> struct SampleResampler<u16, s16>	{ static void Write(u16 &dst, const s16 &src)	{ dst = src + (u16)32768; } };

		template<> struct SampleResampler<s16, u8>	{ static void Write(s16 &dst, const u8 &src)	{ dst = ((s16)src - (s16)128) << 8; } };
		template<> struct SampleResampler<s16, s8>	{ static void Write(s16 &dst, const s8 &src)	{ dst = src << 8; } };
		template<> struct SampleResampler<s16, u16>	{ static void Write(s16 &dst, const u16 &src)	{ dst = src - (u16)32768; } };
		template<> struct SampleResampler<s16, s16>	{ static void Write(s16 &dst, const s16 &src)	{ dst = src; } };

		template<int DstChannels, int SrcChannels>
		struct ChannelResampler {
			template<typename DstType, typename SrcType>
			static void resample(DstType * &dst, const SrcType *src);
		};

		template<>
		struct ChannelResampler<1, 1> {
			template<typename DstType, typename SrcType>
			static void resample(DstType * &dst, const SrcType *src) {
				SampleResampler<DstType, SrcType>::Write(*dst++, *src);
			}
		};
		template<>
		struct ChannelResampler<2, 2> {
			template<typename DstType, typename SrcType>
			static void resample(DstType * &dst, const SrcType *src) {
				SampleResampler<DstType, SrcType>::Write(*dst++, *src++);
				SampleResampler<DstType, SrcType>::Write(*dst++, *src++);
			}
		};
		template<>
		struct ChannelResampler<1, 2> {
			template<typename DstType, typename SrcType>
			static void resample(DstType * &dst, const SrcType *src) {
				SrcType v = (*src++ >> 1);
				v += (*src++ >> 1);
				SampleResampler<DstType, SrcType>::Write(*dst++, v);
			}
		};
		template<>
		struct ChannelResampler<2, 1> {
			template<typename DstType, typename SrcType>
			static void resample(DstType * &dst, const SrcType *src) {
				SampleResampler<DstType, SrcType>::Write(*dst++, *src);
				SampleResampler<DstType, SrcType>::Write(*dst++, *src);
			}
		};

		template<typename DstAudioFormat, int DstChannels, typename SrcAudioFormat, int SrcChannels>
		struct Resampler4 {
			typedef impl::ChannelResampler<DstChannels, SrcChannels> ChannelResampler;

			static void resample(AudioSpec dst_format, Buffer &dst_data, AudioSpec src_format, const Buffer &src_data) {
				const typename SrcAudioFormat::Type *src = static_cast<typename SrcAudioFormat::Type *>(src_data.get_ptr());
				size_t src_size = src_data.get_size() / sizeof(typename SrcAudioFormat::Type) / src_format.channels;
				size_t dst_size = (size_t)(src_size * (1.0f * dst_format.sample_rate / src_format.sample_rate));

				dst_data.resize(dst_size * dst_format.channels * sizeof(typename DstAudioFormat::Type));
				typename DstAudioFormat::Type *dst = static_cast<typename DstAudioFormat::Type *>(dst_data.get_ptr());
				LOG_DEBUG(("converting %d:%u:%u (%u) -> %d:%u:%u (%u)",
					src_format.sample_rate, 8 * (unsigned)sizeof(typename SrcAudioFormat::Type), src_format.channels, (unsigned)src_data.get_size(),
					dst_format.sample_rate, 8 * (unsigned)sizeof(typename DstAudioFormat::Type), dst_format.channels, (unsigned)dst_data.get_size()
				));
				if (src_format.channels == 0)
					throw std::runtime_error("invalid src channel number");
				if (dst_format.channels == 0)
					throw std::runtime_error("invalid dst channel number");

				if (dst_size == 0)
					return;

				int error = 2 * src_size - dst_size;
				for(size_t i = 0; i < dst_size; ++i) {
					ChannelResampler::resample(dst, src);
					if (error > 0) {
						src += src_format.channels;
						error -= 2 * dst_size;
					}
					error += 2 * src_size;
				}
			}
		};

		template<typename DstAudioFormat, int DstChannels, typename SrcAudioFormat>
		struct Resampler3 {
			static void resample(AudioSpec dst_format, Buffer &dst_data, AudioSpec src_format, const Buffer &src_data) {
				switch(src_format.channels) {
					case 1: Resampler4<DstAudioFormat, DstChannels, SrcAudioFormat, 1>::resample(dst_format, dst_data, src_format, src_data); break;
					case 2: Resampler4<DstAudioFormat, DstChannels, SrcAudioFormat, 2>::resample(dst_format, dst_data, src_format, src_data); break;
					default: throw std::runtime_error("invalid dst channel count");
				}
			}
		};

		template<typename DstAudioFormat, typename SrcAudioFormat>
		struct Resampler2 {
			static void resample(AudioSpec dst_format, Buffer &dst_data, AudioSpec src_format, const Buffer &src_data) {
				switch(dst_format.channels) {
					case 1: Resampler3<DstAudioFormat, 1, SrcAudioFormat>::resample(dst_format, dst_data, src_format, src_data); break;
					case 2: Resampler3<DstAudioFormat, 2, SrcAudioFormat>::resample(dst_format, dst_data, src_format, src_data); break;
					default: throw std::runtime_error("invalid dst channel count");
				}
			}
		};

		template<typename DstAudioFormat>
		struct Resampler1 {
			static void resample(AudioSpec dst_format, Buffer &dst, AudioSpec src_format, const Buffer &src)
			{
				switch(src_format.format) {
					case AudioSpec::S8:		Resampler2<DstAudioFormat, AudioFormat<AudioSpec::S8> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::S16:	Resampler2<DstAudioFormat, AudioFormat<AudioSpec::S16> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::U8:		Resampler2<DstAudioFormat, AudioFormat<AudioSpec::U8> >::resample(dst_format, dst, src_format, src); break;
					case AudioSpec::U16:	Resampler2<DstAudioFormat, AudioFormat<AudioSpec::U16> >::resample(dst_format, dst, src_format, src); break;
					default: throw std::runtime_error("invalid src format");
				}
			}
		};
	}

	struct Resample {
		static void resample(AudioSpec dst_format, Buffer &dst, AudioSpec src_format, const Buffer &src)
		{
			switch(dst_format.format) {
				case AudioSpec::S8:		impl::Resampler1<AudioFormat<AudioSpec::S8> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::S16:	impl::Resampler1<AudioFormat<AudioSpec::S16> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::U8:		impl::Resampler1<AudioFormat<AudioSpec::U8> >::resample(dst_format, dst, src_format, src); break;
				case AudioSpec::U16:	impl::Resampler1<AudioFormat<AudioSpec::U16> >::resample(dst_format, dst, src_format, src); break;
				default: throw std::runtime_error("invalid dst format");
			}
		}
	};
}

#endif
