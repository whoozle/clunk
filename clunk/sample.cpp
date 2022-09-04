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

#include <clunk/sample.h>
#include <clunk/context.h>
#include <clunk/locker.h>
#include <clunk/logger.h>
#include <clunk/resample.h>
#include <stdexcept>

using namespace clunk;

Sample::Sample(Context *context) : gain(1.0f), pitch(1.0f), _context(context) {}

void Sample::generateSine(const int freq, const float len) {
	AudioLocker l;

	_spec.sample_rate = _context->get_spec().sample_rate;
	_spec.channels = 1;
	_spec.format = _context->get_spec().format;

	unsigned size = ((int)(len * _spec.sample_rate)) * 2;
	_data.resize(size);

	static double a = 0;
	double da = freq * 2 * M_PI / _spec.sample_rate;
	//LOG_DEBUG(("da = %g", da));

	int n = size / 2;

	s16 * stream = static_cast<s16 *>(_data.get_ptr());
	for(int i = 0; i < n; ++i) {
		*stream++ = (s16)(32767 * sin(a));
		//*stream++ = 0;
		a += da;
	}
	LOG_DEBUG(("generated %u bytes", (unsigned)_data.get_size()));
}

void Sample::init(const clunk::Buffer &src_data, const AudioSpec &spec) {
	AudioLocker l;

	_spec.sample_rate = _context->get_spec().sample_rate;
	_spec.channels = 1;
	_spec.format = _context->get_spec().format;
	Resample::resample(_spec, _data, spec, src_data);
}

Sample::~Sample() { }
