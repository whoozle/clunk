/* libClunk - cross-platform 3D audio API built on top SDL library
 * Copyright (C) 2007-2008 Netive Media Group
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


#define _USE_MATH_DEFINES
#include <math.h>
#include <SDL_rwops.h>
#include "sample.h"
#include "sdl_ex.h"
#include "context.h"
#include "locker.h"

using namespace clunk;

Sample::Sample(Context *context) : gain(1.0f), pitch(1.0f), context(context) {}

void Sample::generateSine(const int freq, const float len) {
	AudioLocker l;
	
	spec.freq = context->get_spec().freq;
	spec.channels = 1;
	spec.format = context->get_spec().format;

	unsigned size = ((int)(len * spec.freq)) * 2;
	data.set_size(size);

	static double a = 0;
	double da = freq * 2 * M_PI / spec.freq;
	//LOG_DEBUG(("da = %g", da));
	
	int n = size / 2;

	Sint16 * stream = (Sint16 *)data.get_ptr();
	for(int i = 0; i < n; ++i) {
		*stream++ = (Sint16)(32767 * sin(a));
		//*stream++ = 0;
		a += da;
	}
	LOG_DEBUG(("generated %u bytes", (unsigned)data.get_size()));
}

void Sample::init(const clunk::Buffer &src_data, int rate, const Uint16 format, const Uint8 channels) {
	AudioLocker l;

	spec.freq = context->get_spec().freq;
	spec.channels = channels;
	spec.format = context->get_spec().format;
	context->convert(data, src_data, rate, format, channels);
}

void Sample::load(const std::string &file) {
	Uint8 *buf;
	Uint32 len;
	//SDL_AudioSpec * SDLCALL SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
	if (SDL_LoadWAV(file.c_str(), &spec, &buf, &len) == NULL)
		throw_sdl(("SDL_LoadWav"));

	clunk::Buffer wav;
	wav.set_data(buf, len, true);
	context->convert(data, wav, spec.freq, spec.format, spec.channels);
	
	name = file;
}


Sample::~Sample() {
	
}
