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

