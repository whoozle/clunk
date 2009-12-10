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


#include <SDL.h>
#include <SDL_audio.h>
#include "context.h"
#include <string.h>
#include "sdl_ex.h"
#include "logger.h"
#include "source.h"
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <map>
#include <algorithm>
#include <vector>
#include "locker.h"
#include "stream.h"
#include "object.h"

using namespace clunk;

Context::Context() : period_size(0), listener(NULL), max_sources(8), fx_volume(1), distance_model(DistanceModel::Inverse, true, 128), fdump(NULL) {
}

void Context::callback(void *userdata, Uint8 *bstream, int len) {
	Context *self = (Context *)userdata;
	assert(self != NULL);
	Sint16 *stream = (Sint16*)bstream;
	TRY {
		self->process(stream, len);
	} CATCH("callback", {})
}

namespace clunk {
struct source_t {
	Source *source;
	
	v3<float> s_pos;
	v3<float> s_vel;
	v3<float> s_dir;
	v3<float> l_vel;

	inline source_t(Source *source, const v3<float> &s_pos, const v3<float> &s_vel, const v3<float>& s_dir, const v3<float>& l_vel) : 
		source(source), s_pos(s_pos), s_vel(s_vel), s_dir(s_dir), l_vel(l_vel) {}
};
}

void Context::process(Sint16 *stream, int size) {
	//TIMESPY(("total"));

	v3<float> l_pos, l_vel;
	if (listener != NULL) {
		l_pos = listener->position;
		l_vel = listener->velocity;
	}
	{
		//TIMESPY(("sorting objects"));
		std::sort(objects.begin(), objects.end(), Object::DistanceOrder(l_pos));
	}
	//LOG_DEBUG(("sorted %u objects", (unsigned)objects.size()));
	
	std::vector<source_t> lsources;
	int n = size / 2 / spec.channels;
	typedef std::map<const std::string, unsigned> stats_type;
	stats_type sources_stats;
	
	for(objects_type::iterator i = objects.begin(); i != objects.end(); ) {
		Object *o = *i;
		Object::Sources & sset = o->sources;
		if (sset.empty() && o->dead) {
			//autodeleted object
			delete o;
			i = objects.erase(i);
			continue;
		}
		for(Object::Sources::iterator j = sset.begin(); j != sset.end(); ) {
			const std::string &name = j->first;
			Source *s = j->second;
			if (!s->playing()) {
				//LOG_DEBUG(("purging inactive source %s", j->first.c_str()));
				delete j->second;
				sset.erase(j++);
				continue;
			}
			
			stats_type::iterator s_i = sources_stats.find(name);
			unsigned same_sounds_n = (s_i != sources_stats.end())? s_i->second: 0;

			if (lsources.size() < max_sources && same_sounds_n < distance_model.same_sounds_limit) {
				lsources.push_back(source_t(s, o->position + s->delta_position - l_pos, o->velocity, o->direction, l_vel));
				if (same_sounds_n == 0) {
					sources_stats.insert(stats_type::value_type(name, 1));
				} else {
					++s_i->second;
				}
				//LOG_DEBUG(("%u: source: %s", (unsigned)lsources.size(), name.c_str()));
			} else {
				s->update_position(n);
			}
			++j;
		}
		++i;
	}

	memset(stream, 0, size);

	for(streams_type::iterator i = streams.begin(); i != streams.end();) {
		//LOG_DEBUG(("processing stream %d", i->first));
		stream_info &stream_info = i->second;
		while ((int)stream_info.buffer.get_size() < size) {
			clunk::Buffer data;
			bool eos = !stream_info.stream->read(data, size);
			if (!data.empty() && stream_info.stream->sample_rate != spec.freq) {
				//LOG_DEBUG(("converting audio data from %u to %u", stream_info.stream->sample_rate, spec.freq));
				convert(data, data, stream_info.stream->sample_rate, stream_info.stream->format, stream_info.stream->channels);
			}
			stream_info.buffer.append(data);
			//LOG_DEBUG(("read %u bytes", (unsigned)data.get_size()));
			if (eos) {
				if (stream_info.loop) {
					stream_info.stream->rewind();
				} else {
					break;
				}
			}
		}
		int buf_size = (int)stream_info.buffer.get_size();
		//LOG_DEBUG(("buffered %d bytes", buf_size));
		if (buf_size == 0) {
			//all data buffered. continue;
			LOG_DEBUG(("stream %d finished. dropping.", i->first));
			TRY {
				delete stream_info.stream;
			} CATCH("mixing stream", {});
			streams.erase(i++);
			continue;
		}
		
		if (buf_size >= size)
			buf_size = size;

		int sdl_v = (int)floor(SDL_MIX_MAXVOLUME * stream_info.gain + 0.5f);
		SDL_MixAudio((Uint8 *)stream, (Uint8 *)stream_info.buffer.get_ptr(), buf_size, sdl_v);
		
		if ((int)stream_info.buffer.get_size() > size) {
			memmove(stream_info.buffer.get_ptr(), ((Uint8 *)stream_info.buffer.get_ptr()) + size, stream_info.buffer.get_size() - size);
			stream_info.buffer.set_size(stream_info.buffer.get_size() - size);
		} else {
			stream_info.buffer.free();
		}
		
		++i;
	}
	
	clunk::Buffer buf;
	buf.set_size(size);
	
	//TIMESPY(("mixing sources"));
	//LOG_DEBUG(("mixing %u sources", (unsigned)lsources.size()));
	for(unsigned i = 0; i < lsources.size(); ++i ) {
		const source_t& source_info = lsources[i];
		Source * source = source_info.source;
				
		float dpitch = 1.0f;
		if (distance_model.doppler_factor > 0) {
			dpitch = distance_model.doppler_pitch(-source_info.s_pos, source_info.s_vel, source_info.l_vel);
		}

		float volume = fx_volume * distance_model.gain(source_info.s_pos.length());
		int sdl_v = (int)floor(SDL_MIX_MAXVOLUME * volume + 0.5f);
		if (sdl_v <= 0)
			continue;
		//check for 0
		volume = source->process(buf, spec.channels, source_info.s_pos, source_info.s_dir, volume, dpitch);
		sdl_v = (int)floor(SDL_MIX_MAXVOLUME * volume + 0.5f);
		//LOG_DEBUG(("%u: mixing source with volume %g (%d)", i, volume, sdl_v));
		if (sdl_v <= 0)
			continue;
		if (sdl_v > SDL_MIX_MAXVOLUME)
			sdl_v = SDL_MIX_MAXVOLUME;
		
		SDL_MixAudio((Uint8 *)stream, (Uint8 *)buf.get_ptr(), size, sdl_v);
	}
	
	if (fdump != NULL) {
		if (fwrite(stream, size, 1, fdump) != 1) {
			fclose(fdump);
			fdump = NULL;
		}
	}
}


Object *Context::create_object() {
	AudioLocker l;
	Object *o = new Object(this);
	objects.push_back(o);
	return o;
}

Sample *Context::create_sample() {
	AudioLocker l;
	return new Sample(this);
}

void Context::save(const std::string &file) {
	AudioLocker l;
	if (fdump != NULL) {
		fclose(fdump);
		fdump = NULL;
	}
	if (file.empty())
		return;
	
	fdump = fopen(file.c_str(), "wb");
}

void Context::init(const int sample_rate, const Uint8 channels, int period_size) {
	if (!SDL_WasInit(SDL_INIT_AUDIO)) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
			throw_sdl(("SDL_InitSubSystem"));
	}
	
	SDL_AudioSpec src;
	memset(&src, 0, sizeof(src));
	src.freq = sample_rate;
	src.channels = channels;
	src.format = AUDIO_S16SYS;
	src.samples = period_size;
	src.callback = &Context::callback;
	src.userdata = (void *) this;
	
	this->period_size = period_size;
	
	if ( SDL_OpenAudio(&src, &spec) < 0 )
		throw_sdl(("SDL_OpenAudio(%d, %u, %d)", sample_rate, channels, period_size));
	if (spec.format != AUDIO_S16SYS)
		throw_ex(("SDL_OpenAudio(%d, %u, %d) returned format %d", sample_rate, channels, period_size, spec.format));
	if (spec.channels < 2)
		LOG_ERROR(("Could not operate on %d channels", spec.channels));

	LOG_DEBUG(("opened audio device, sample rate: %d, period: %d, channels: %d", spec.freq, spec.samples, spec.channels));
	SDL_PauseAudio(0);
	
	AudioLocker l;
	listener = create_object();
}

void Context::delete_object(Object *o) {
	AudioLocker l;
	objects_type::iterator i = std::find(objects.begin(), objects.end(), o);
	while(i != objects.end() && *i == o)
		i = objects.erase(i); //just for fun
}

void Context::deinit() {
	//cleanup objects here too.
	if (!SDL_WasInit(SDL_INIT_AUDIO))
		return;
	
	AudioLocker l;
	delete listener;
	listener = NULL;
	SDL_CloseAudio();
	
	if (fdump != NULL) {
		fclose(fdump);
		fdump = NULL;
	}

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
	
Context::~Context() {
	deinit();
}


//MUSIC MIXER: 

void Context::play(const int id, Stream *stream, bool loop) {
	LOG_DEBUG(("play(%d, %p, %s)", id, (const void *)stream, loop?"'loop'":"'once'"));
	AudioLocker l;
	stream_info & stream_info = streams[id];
	delete stream_info.stream;
	stream_info.stream = stream;
	stream_info.loop = loop;
	stream_info.paused = false;
	stream_info.gain = 1.0f;
}

bool Context::playing(const int id) const {
	AudioLocker l;
	return streams.find(id) != streams.end();
}

void Context::pause(const int id) {
	AudioLocker l;
	streams_type::iterator i = streams.find(id);
	if (i == streams.end())
		return;
	
	i->second.paused = !i->second.paused;
}

void Context::stop(const int id) {
	AudioLocker l;
	streams_type::iterator i = streams.find(id);
	if (i == streams.end())
		return;
	
	TRY {
		delete i->second.stream;
	} CATCH(clunk::format_string("stop(%d)", id).c_str(), {
		streams.erase(i);
		throw;
	})
	streams.erase(i);
}

void Context::set_volume(const int id, float volume) {
	if (volume < 0)
		volume = 0;
	if (volume > 1)
		volume = 1;
		
	streams_type::iterator i = streams.find(id);
	if (i == streams.end())
		return;
	i->second.gain = volume;
}

void Context::set_fx_volume(float volume) {
	//LOG_WARN(("ignoring set_fx_volume(%g)", volume));
	if (volume  < 0)	
		fx_volume = 0;
	else if (volume > 1)
		fx_volume = 1;
	else 
		fx_volume = volume;
}

void Context::stop_all() {
	AudioLocker l;
	for(streams_type::iterator i = streams.begin(); i != streams.end(); ++i) {
		delete i->second.stream;
	}
	streams.clear();
}

void Context::set_max_sources(int sources) {
	AudioLocker l;
	max_sources = sources;
}

void Context::convert(clunk::Buffer &dst, const clunk::Buffer &src, int rate, const Uint16 format, const Uint8 channels) {
	SDL_AudioCVT cvt;
	memset(&cvt, 0, sizeof(cvt));
	if (SDL_BuildAudioCVT(&cvt, format, channels, rate, spec.format, channels, spec.freq) == -1) {
		throw_sdl(("DL_BuildAudioCVT(%d, %04x, %u)", rate, format, channels));
	}
	size_t buf_size = (size_t)(src.get_size() * cvt.len_mult);
	cvt.buf = (Uint8 *)malloc(buf_size);
	cvt.len = (int)src.get_size();

	assert(buf_size >= src.get_size());
	memcpy(cvt.buf, src.get_ptr(), src.get_size());

	if (SDL_ConvertAudio(&cvt) == -1) 
		throw_sdl(("SDL_ConvertAudio"));

	dst.set_data(cvt.buf, (size_t)(cvt.len * cvt.len_ratio), true);
}

/*!
	\mainpage Tutorial 
	\section overview Overview
	Hello there! 
	Here's quick explanation of the clunk library concepts and usage scenarios. 
	\section scenario Typical scenario

	First of all, initialize SDL in your code: 
	\code
	SDL_Init(SDL_INIT_AUDIO) or SDL_InitSubSystem(SDL_INIT_AUDIO);
	\endcode

	Let's initialize context with typical values: 22kHz sample rate, 2 channels and 1024 bytes period: 
	\code
	Context context; 
	context.init(22050, 2, 1024);
	//main code
	context.deinit();
	\endcode
	If you choose greater sample rate such as 44kHz or even 48kHz, you will need more CPU power to mix sounds and it could hurt overall game performance. 
	You could raise period value to avoid clicks, but you get more latency for that. 
	Latency could be calculated with the following formula: 
	\code latency (in seconds) = period_size / channels / byte per sample (2 for 16 bit sound) / sample_rate \endcode
	in this example latency is only 12ms. Such small delays are almost invisible even for perfect ears :)
	
	Then application should load some samples to the library. Clunk itself does not provide code to decode audio formats, or load raw wave files. 
	Check ogg/vorbis library for a free production-quality audio codec. Samples allocates within context internally with clunk::Context::create_sample() method. 

	\code
	clunk::Buffer data; //placeholder for a memory chunk
	//decode ogg sample into data
	clunk::Sample *sample = Context->create_sample();
	sample->init(data, ogg_rate, AUDIO_S16LSB, ogg_channels);
	\endcode
	
	So all audio data were loaded and initialized. Next step is to allocate objects. Clunk was designed to be easily integrated into programs. 
	The most useful object is clunk::Object. It could hold several playing \link clunk::Source sources \endlink. 
	You could use two different approaches here: 
		\li create global mixer proxy object and leave all clunk stuff to it, such as mapping your objects to clunk ones. 
		\li directly include clunk::Object pointer into every object in game or program. 

	You wont ever need to track objects and/or manage its destruction, clunk will do it itself. 
	Example allowing sound to play after your object's death: 
	\code
	clunk::Object *clunk_object; 
	
	GameObject::~GameObject() {
		if (clunk_object != NULL) {
			clunk_object->autodelete(); //destroy me! 
			clunk_object = NULL; //leave destruction to the clunk::Context
		}
	}
	\endcode
	
	So the next step is source management. It's the most easiest part. Each source connects to its audio sample. 
	Source holds data about actual playing sound: position in wave data, pitch, gain and distance. It processes audio data 
	and simulate 3d sound positioning with hrtf function. 
	
	Creating source and adding it to the object : (the most easiest part)
	\code
		clunk_object->play("voice", new Source(yeti_sound_sample)); // no loop, no pitch, no gain adjustments. 
	\endcode
	Sources are automatically purged from the object when they are not needed anymore. So, you don't need to worry about its deletion or any management. 
	Anyway, you could cancel any playing source: 
	\code 
		clunk_object->cancel("voice"); 
	\endcode
	
	Or cancel all sounds from this object at once: 
	\code 
		clunk_object->cancel_all(true);
	\endcode
	
	\section positioning Object positioning
	Usually objects are positioning the some sort of ticking function called every frame or from the on_object_update callback. 
	Positioning is really simple: 
	\code
		clunk_object->update(clunk::v3<float>(x, y, z), clunk::v3<float>(velocity_x, velocity_y, velocity_z), clunk::v3<float>(direction_x, direction_y, direction_z));
	\endcode
	Moving listener is easy too, listener is regular clunk::Object, but it's stored in clunk::Context and holds information about your position
	\code
		context.get_listener()->update(clunk::v3<float>(x, y, z), clunk::v3<float>(velocity_x, velocity_y, velocity_z), clunk::v3<float>(direction_x, direction_y, direction_z));
	\endcode
	
	\section streaming Playing music and ambient sounds
	Clunk is able to mix as many music streams as you want (or your CPU could handle :) ). 
	First of all you need to implement your stream class derived from the clunk::Stream. 
	Don't worry, you need to implement just 2(!) clunk-related methods to make the music play. 
	\code 
		class FooStream : public clunk::Stream {
		public: 
			void FooStream(const std::string &file) {
				//open music file. 
				//store music parameters into members : 
				sample_rate = music_rate;
				channels = music_channels;
				format = AUDIO_S16LSB; 
				//this values here are only for educational purpose. Don't forget to fill it with actual values from the music file!
			}
			
			void rewind() {
				//rewind your stream here
			}
			
			bool read(clunk::Buffer &data, unsigned hint) {
				//read as many data as you want, but it'd better to read around 'hint' bytes to avoid memory queue overhead. 
			}

			virtual ~FooStream() {
				//don't forget to close your stream here. Leaks are unwanted guests here. 
			}
		};
	\endcode
	
	So, the most complicated part passed by. Let the party begin !
	\code 
		context.play(0, new FooStream("data/background_music.ogg"), false); //do not loop music, look below for details.
		context.play(1, new FooStream("data/ambience_city.ogg"), true); //loops ambient
	\endcode

	There's no magic numbers here. I've chosen 0 and 1 just for fun. You could use any integer id. 42 for example. 
	Why don't I use loop == true for music ? We need it to change various tunes. Let's periodically test if music ends and restart with new tune: 
	\code 
		if (!context.playing(0)) {
			context.play(0, new FooStream(next_song));
		}
	\endcode
	
	\section final Final words from author
	I've covered almost all major topics of the clunk here in this tutorial. If you have suggestion - feel free to contact me directly. 
	Hope all this code will be useful for someone. Good luck! We're waiting for your feedback!
	
*/
