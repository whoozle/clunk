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


#ifndef CLUNK_CONTEXT_H__
#define CLUNK_CONTEXT_H__

#include <map>
#include <deque>
#include <stdio.h>
#include <SDL_audio.h>

#include "export_clunk.h"
#include "object.h"
#include "sample.h"
#include "buffer.h"
#include "distance_model.h"

namespace clunk {

class Stream;

/*! 
	\brief Clunk context, main class for the audio output and mixing.
	Main class for the clunk library. Holds audio callback and generates sound. 
	Also, mantains audio streams.
*/

class CLUNKAPI Context {
public: 
	Context();
	
	/*! 
		\brief Initializes clunk context. 
		\param[in] sample_rate sample rate of the audio output
		\param[in] channels audio output channels number, supported values 1 or 2 for now. 
		\param[out] period_size minimal processing unit (bytes). Less period - less latency.
	*/
	void init(const int sample_rate, const Uint8 channels, int period_size);
	/*! 
		\brief Sets maximum simultaneous sources number. 
		Do not use values that are too high. Use reasonable default such as 8 or 16 
		\param[in] sources maximum simultaneous sources
	*/
	void set_max_sources(int sources);
	
	//saves raw stream into file. use save(std::string()) to stop this madness.
	void save(const std::string &file);

	///stops any sound generation and shuts down SDL subsystem
	void deinit();

	///creates new clunk::Object
	Object *create_object();

	///creates clunk::Sample 
	Sample *create_sample();
	
	/// dtor, deinits context automatically if you forget :)
	~Context();
	
	/*! 
		\brief returns audio specification for output
		\return output audio specification in SDL format. 
	*/
	const SDL_AudioSpec & get_spec() const {
		return spec;
	}
	
	///internal: NEVER USE IT !
	/*!
		\internal generate next 'len' bytes
	*/
	void process(Sint16 *stream, int len);
	/*! 
		\brief plays stream with given id. 
		\param[in] id stream id - any integer you want. 
		\param[in] stream stream object, see clunk::Stream documentation for the details.
		\param[in] loop auto rewind stream after it ends
	*/
	void play(const int id, Stream *stream, bool loop);
	///returns stream's status
	bool playing(const int id) const;
	///pauses stream with given id
	void pause(const int id);
	///stops stream with given id
	void stop(const int id);
	/*!
		\brief sets volume for stream
		\param[in] id stream id
		\param[in] volume volume (0.0 - 1.0)
	*/
	void set_volume(const int id, float volume);
	
	/*! 
		\brief sets volume of the generated sound
		\param[in] volume volume of the 3d-sounds (global fx volume, 0.0 - 1.0)
	*/
	void set_fx_volume(float volume);
	/*!
		\brief stops all sources.
	*/
	void stop_all();

	/*!
		\brief converts raw audio data from one format to the current audio format
		\param[out] dst destination data
		\param[in] src source data
		\param[in] rate sample rate of the source data
		\param[in] format SDL audio format. See SDL_audio.h or SDL documentation for the details.
		\param[in] channels source channels. 
	*/
	void convert(clunk::Buffer &dst, const clunk::Buffer &src, int rate, const Uint16 format, const Uint8 channels);
	
	///returns object associated to the current listener position
	Object *get_listener() { return listener; }
	
	///Sets distance model
	inline void set_distance_model(const DistanceModel &model) { distance_model = model; }
	DistanceModel &get_distance_model() { return distance_model; }

private: 
	SDL_AudioSpec spec;
	int period_size;

	static void callback(void *userdata, Uint8 *stream, int len);
	void delete_object(Object *o);

	friend clunk::Object::~Object();
	friend clunk::Sample::~Sample();
	
	typedef std::deque<Object *> objects_type;
	objects_type objects;
	
	struct stream_info {
		stream_info() : stream(NULL), loop(false), gain(1.0f), paused(false), buffer() {}
		Stream *stream;
		bool loop;
		float gain;
		bool paused;
		clunk::Buffer buffer;
	};
	
	typedef std::map<const int, stream_info> streams_type;
	streams_type streams;

	Object *listener;
	unsigned max_sources;
	float fx_volume;
	
	DistanceModel distance_model;
	
	FILE * fdump;
};
}


#endif

