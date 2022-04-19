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

#ifndef CLUNK_SAMPLE_H__
#define CLUNK_SAMPLE_H__

#include <clunk/export_clunk.h>
#include <clunk/buffer.h>
#include <clunk/audio_spec.h>

namespace clunk {
class Context;

//!Holds raw wave data. 
class CLUNKAPI Sample {
public: 
	///name - for general purpose
	std::string name;
	///gain
	float gain;
	///pitch, 2.0f - pitching up one octave
	float pitch;

	~Sample();

	/*!
		\brief initializes sample
		\param[in] data raw audio data
		\param[in] spec audio format specification
	*/	
	void init(const clunk::Buffer &data, const AudioSpec &spec);

	/*! 
		\brief generate sine wave with given length (seconds)
		\param[in] freq frequency
		\param[in] len of the sample in seconds
	*/
	void generateSine(int freq, float len);

	float length() const {
		return 1.0f * _data.get_size() / _spec.sample_rate / _spec.channels / 2;
	}

	const clunk::Buffer & get_data() const	{ return _data; }
	const AudioSpec get_spec() const		{ return _spec; }

private:
	friend class Context;

	Sample(Context *context);

	Sample(const Sample &);
	const Sample& operator=(const Sample &);

	Context *		_context;
	AudioSpec		_spec;
	clunk::Buffer	_data;
};
}

#endif

