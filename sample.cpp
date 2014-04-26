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


#include "sample.h"
#include "context.h"
#include "locker.h"
#include "logger.h"
#include <stdexcept>

using namespace clunk;

Sample::Sample(Context *context) : gain(1.0f), pitch(1.0f), _context(context) {}

void Sample::generateSine(const int freq, const float len) {
	AudioLocker l;
	
	_spec.freq = _context->get_spec().freq;
	_spec.channels = 1;
	_spec.format = _context->get_spec().format;

	unsigned size = ((int)(len * _spec.freq)) * 2;
	_data.set_size(size);

	static double a = 0;
	double da = freq * 2 * M_PI / _spec.freq;
	//LOG_DEBUG(("da = %g", da));
	
	int n = size / 2;

	Sint16 * stream = (Sint16 *)_data.get_ptr();
	for(int i = 0; i < n; ++i) {
		*stream++ = (Sint16)(32767 * sin(a));
		//*stream++ = 0;
		a += da;
	}
	LOG_DEBUG(("generated %u bytes", (unsigned)_data.get_size()));
}

void Sample::init(const clunk::Buffer &src_data, const AudioSpec &spec) {
	AudioLocker l;

	_spec.freq = _context->get_spec().freq;
	_spec.channels = spec.channels;
	_spec.format = _context->get_spec().format;
	_context->convert(_data, src_data, spec);
}

Sample::~Sample() { }
