#ifndef CLUNK_BACKEND_SDL_H
#define CLUNK_BACKEND_SDL_H

#ifndef BACKEND_SDL
#	error invalid configuration
#endif

#include <clunk/sample.h>
#include <clunk/context.h>
#include <SDL.h>
#include <SDL_audio.h>

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

