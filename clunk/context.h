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

#ifndef CLUNK_CONTEXT_H__
#define CLUNK_CONTEXT_H__

#include <map>
#include <deque>
#include <vector>
#include <stdio.h>

#include <clunk/export_clunk.h>
#include <clunk/object.h>
#include <clunk/sample.h>
#include <clunk/buffer.h>
#include <clunk/distance_model.h>

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
		\param[in] spec specification of audio format
	*/
	void init(const AudioSpec &spec);
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
	const AudioSpec & get_spec() const {
		return _spec;
	}
	
	///internal: NEVER USE IT !
	/*!
		\internal generate next 'len' bytes
	*/
	void process(void *stream, size_t len);
	/*! 
		\brief plays stream with given id. 
		\param[in] id stream id - any integer you want. 
		\param[in] stream stream object, see clunk::Stream documentation for the details.
		\param[in] loop auto rewind stream after it ends
	*/
	void play(int id, Stream *stream, bool loop);
	///returns stream's status
	bool playing(int id) const;
	///pauses stream with given id
	void pause(int id);
	///stops stream with given id
	void stop(int id);
	/*!
		\brief sets volume for stream
		\param[in] id stream id
		\param[in] volume volume (0.0 - 1.0)
	*/
	void set_volume(int id, float volume);
	
	/*! 
		\brief sets volume of the generated sound
		\param[in] volume volume of the 3d-sounds (global fx volume, 0.0 - 1.0)
	*/
	void set_fx_volume(float volume);
	/*!
		\brief stops all sources.
	*/
	void stop_all();

	///returns object associated to the current listener position
	ListenerObject *get_listener() { return _listener; }
	
	///Sets distance model
	inline void set_distance_model(const DistanceModel &model) { distance_model = model; }
	DistanceModel &get_distance_model() { return distance_model; }

private:
	AudioSpec _spec;

	void delete_object(Object *o);

	friend clunk::Object::~Object();
	friend clunk::Sample::~Sample();
	
	typedef std::deque<Object *> objects_type;
	objects_type objects;
	
	struct stream_info {
		stream_info() : stream(nullptr), loop(false), gain(1.0f), paused(false), buffer() {}
		Stream *stream;
		bool loop;
		float gain;
		bool paused;
		clunk::Buffer buffer;
	};
	
	typedef std::map<const int, stream_info> streams_type;
	streams_type streams;

	ListenerObject *_listener;
	unsigned max_sources;
	float fx_volume;
	
	DistanceModel distance_model;
	
	FILE * _fdump;

	struct source_t {
		Source *source;
	
		v3f s_pos;
		v3f s_vel;
		v3f l_vel;

		inline source_t(Source *source, const v3f &s_pos, const v3f &s_vel, const v3f& l_vel):
		source(source), s_pos(s_pos), s_vel(s_vel), l_vel(l_vel) {}
	};
	template<class Sources>
	bool process_object(Object *o, Sources &sset, std::vector<source_t> &lsources, unsigned n);
};
}


#endif

