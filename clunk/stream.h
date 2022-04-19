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

#ifndef CLUNK_STREAM_H__
#define CLUNK_STREAM_H__

#include <clunk/export_clunk.h>
#include <clunk/types.h>
#include <clunk/audio_spec.h>

namespace clunk {
class Context;
class Buffer;

/*! 
	\brief Music/Ambient stream.
	simple abstract class allowing you to play audio streams. Note that stream's methods will be called from the audio callback. 
	Usually it's the different thread context and if you're using any global variables from the stream code, you need to protect it with mutex
	or clunk::AudioLocker. 
*/

class CLUNKAPI Stream {
public: 
	Stream();
	///rewinds stream
	/*!
		Rewind your stream in your function
	*/
	virtual void rewind() = 0;
	
	/*! 
		\brief reads some data from the stream. 
		\param[out] data destination buffer
		\param[in] hint points out for the recommented data size. You could read less or more hint. 
	*/
	virtual bool read(clunk::Buffer &data, unsigned hint) = 0;
	virtual ~Stream();

protected: 
	///stream sample rate
	/*! initialize it from your open() method */
	AudioSpec _spec;

	friend class Context;
};
}

#endif

