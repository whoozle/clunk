#ifndef CLUNK_SAMPLE_H__
#define CLUNK_SAMPLE_H__

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


#include <SDL_audio.h>
#include "export_clunk.h"
#include "buffer.h"

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
		\param[in] rate sample rate
		\param[in] format SDL audio format. Look SDL_audio.h or SDL documentation. 
		\param[in] channels audio channels
	*/	
	void init(const clunk::Buffer &data, int rate, const Uint16 format, const Uint8 channels);
	/*!
		\brief loads sample from file
	*/
	void load(const std::string &file);
	/*! 
		\brief generate sine wave with given length (seconds)
		\param[in] freq frequency
		\param[in] len of the sample in seconds
	*/
	void generateSine(const int freq, const float len);
	
	float length() const {
		return 1.0f * data.get_size() / spec.freq / spec.channels / 2;
	}
	
private: 	
	friend class Context;
	friend class Source;
	
	Sample(Context *context);

	Sample(const Sample &);
	const Sample& operator=(const Sample &);

	Context *context;
	SDL_AudioSpec spec;
	clunk::Buffer data;
};
}

#endif

