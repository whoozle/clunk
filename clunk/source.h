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

#ifndef CLUNK_SOURCE_H__
#define CLUNK_SOURCE_H__

#include <clunk/export_clunk.h>
#include <clunk/v3.h>
#include <clunk/buffer.h>
#include <clunk/hrtf.h>

namespace clunk
{

	class Sample;
	class Buffer;

	//!class holding information about source.
	class CLUNKAPI Source
	{
	public:
		///pointer to the sample holding audio data
		const Sample * const sample;

		///loop flag
		bool loop;
		///delta position from the object's center
		v3f delta_position; //0 - from the center of the object.
		///gain
		float gain;
		///pitch, 2.0f - pitching up one octave
		float pitch;
		/*!
				panning. -1.0f = 2 * vol in left channel, 0 in right. +1.0f = no sound in left channel, 2 * vol in right.
				note: panning is actually applied on mono samples in center(listener) position.
		*/
		float panning;
		/*!
				\brief constructs new source
				\param[in] sample audio data
				\param[in] loop loops sample
				\param[in] delta delta position. (0, 0) is the object's center.
				\param[in] gain gain
				\param[in] pitch pitch
		*/
		Source(const Sample * sample, const bool loop = false, const v3f &delta = v3f(), float gain = 1, float pitch = 1, float panning = 0);
		///returns current source's status.
		bool playing() const;

		///fades out source. usually you do not need this method
		void fade_out(float sec);

		~Source();

		/*!
				\brief for the internal use only. DO NOT USE IT.
				\internal for the internal use only.
		*/
		void _update_position(int dp);

		/*!
				\brief for the internal use only. DO NOT USE IT.
				\internal for the internal use only.
		*/
		float _process(clunk::Buffer &buffer, unsigned ch, const v3f &position, float fx_volume, float pitch);

	private:
		int position, fadeout, fadeout_total;
		Hrtf _hrtf;
	};
}

#endif
