#ifndef CLUNK_BACKEND_SDL_H
#define CLUNK_BACKEND_SDL_H

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


#include <clunk/config.h>
#include <clunk/sample.h>
#include <clunk/context.h>
#include <SDL.h>
#include <SDL_audio.h>

#ifndef CLUNK_BACKEND_SDL
#	error invalid configuration
#endif


namespace clunk { namespace sdl {

class CLUNKAPI Backend {
	Context _context;
public:
	/*! 
		\brief Initializes clunk context. 
		\param[in] sample_rate sample rate of the audio output
		\param[in] channels audio output channels number, supported values 1 or 2 for now. 
		\param[out] period_size minimal processing unit (bytes). Less period - less latency.
	*/
	Backend(int sample_rate, const u8 channels, int period_size);
	~Backend();

	/*!
		\brief loads sample from file
	*/
	Sample *load(const std::string &fname);
	void start();
	void stop();

	///gets context
	Context &get_context() { return _context; }

	SDL_AudioSpec convert(const AudioSpec &spec);
	AudioSpec convert(const SDL_AudioSpec &spec);

private:
	static void callback(void *userdata, u8 *stream, int len);
	SDL_AudioSpec _spec;
};

}}


#endif

