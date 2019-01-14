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

#ifndef CLUNK_MIXER_H
#define CLUNK_MIXER_H

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


#include <clunk/audio_spec.h>
#include <limits>

namespace clunk {

	static const int MaxMixVolumeShift = 7;
	static const u8 MaxMixVolume = (1 << MaxMixVolumeShift);

	namespace impl {

		template<typename Format, bool Additive>
		struct SampleMixer;

		template<typename Format>
		struct SampleMixer<Format, true> {
			typedef typename Format::Type			Type;
			typedef typename Format::DoubleType		DoubleType;

			inline DoubleType operator()(const Type dst, const Type src, DoubleType volume) {
				return Format::clip((
					((DoubleType)dst << MaxMixVolumeShift) +
					(volume * (DoubleType)src )
				) >> MaxMixVolumeShift);
			}

			inline DoubleType operator()(const Type dst, const Type src) {
				return dst + src;
			}
		};

		template<typename Format>
		struct SampleMixer<Format, false> {
			typedef typename Format::Type			Type;
			typedef typename Format::DoubleType		DoubleType;

			inline DoubleType operator()(const Type dst, const Type src, DoubleType volume) {
				return Format::clip((
					((DoubleType)dst << MaxMixVolumeShift) -
					(volume * (DoubleType)src )
				) >> MaxMixVolumeShift);
			}

			inline DoubleType operator()(const Type dst, const Type src) {
				return dst - src;
			}
		};

		template<typename Format, bool Additive = true> struct Mixer {

			typedef typename Format::Type			Type;
			typedef typename Format::DoubleType		DoubleType;

			static void mix(void *dst_, const void *src_, size_t size, DoubleType volume) {
				SampleMixer<Format, Additive> mixer;
				size /= sizeof(Type);
				Type *dst = static_cast<Type *>(dst_);
				const Type *src = static_cast<const Type *>(src_);
				if (volume == MaxMixVolume) {
					while(size--) {
						DoubleType value = mixer(*dst, *src++);
						*dst++ = Type(value);
					}
				} else {
					while(size--) {
						DoubleType value = mixer(*dst, *src++, volume);
						if (value > std::numeric_limits<Type>::max())
							value = std::numeric_limits<Type>::max();
						if (value < std::numeric_limits<Type>::min())
							value = std::numeric_limits<Type>::min();
						*dst++ = Type(value);
					}
				}
			}

			static void adjust(void *dst_, size_t size, DoubleType volume) {
				size /= sizeof(Type);
				Type *dst = static_cast<Type *>(dst_);
				while(size--) {
					DoubleType value = ((DoubleType)*dst * volume) >> MaxMixVolumeShift;
					*dst++ = value;
				}
			}
		};
	}

	struct Mixer {
		static void mix(AudioSpec::Format format, void *dst, const void *src, size_t size, int volume = MaxMixVolume)
		{
			switch(format)
			{
				case AudioSpec::S8:		impl::Mixer<AudioFormat<AudioSpec::S8> >::mix(dst, src, size, volume); break;
				case AudioSpec::S16:	impl::Mixer<AudioFormat<AudioSpec::S16> >::mix(dst, src, size, volume); break;
				case AudioSpec::U8:		impl::Mixer<AudioFormat<AudioSpec::U8> >::mix(dst, src, size, volume); break;
				case AudioSpec::U16:	impl::Mixer<AudioFormat<AudioSpec::U16> >::mix(dst, src, size, volume); break;
			}
		}

		static void add(AudioSpec::Format format, void *dst, const void *src, size_t size, int volume = MaxMixVolume)
		{ mix(format, dst, src, size, volume); }

		static void sub(AudioSpec::Format format, void *dst, const void *src, size_t size, int volume = MaxMixVolume)
		{
			switch(format)
			{
				case AudioSpec::S8:		impl::Mixer<AudioFormat<AudioSpec::S8>, false>::mix(dst, src, size, volume); break;
				case AudioSpec::S16:	impl::Mixer<AudioFormat<AudioSpec::S16>, false>::mix(dst, src, size, volume); break;
				case AudioSpec::U8:		impl::Mixer<AudioFormat<AudioSpec::U8>, false>::mix(dst, src, size, volume); break;
				case AudioSpec::U16:	impl::Mixer<AudioFormat<AudioSpec::U16>, false>::mix(dst, src, size, volume); break;
			}
		}

		static void adjust_volume(AudioSpec::Format format, void *dst, size_t size, int volume)
		{
			switch(format)
			{
				case AudioSpec::S8:		impl::Mixer<AudioFormat<AudioSpec::S8> >::adjust(dst, size, volume); break;
				case AudioSpec::S16:	impl::Mixer<AudioFormat<AudioSpec::S16> >::adjust(dst, size, volume); break;
				case AudioSpec::U8:		impl::Mixer<AudioFormat<AudioSpec::U8> >::adjust(dst, size, volume); break;
				case AudioSpec::U16:	impl::Mixer<AudioFormat<AudioSpec::U16> >::adjust(dst, size, volume); break;
			}
		}
	};
}

#endif
