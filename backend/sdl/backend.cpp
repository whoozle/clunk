#include "backend.h"
#include "sdl_ex.h"
#include <SDL_rwops.h>
#include <SDL_audio.h>
#include <SDL.h>
#include "sdl_ex.h"
#include "context.h"
#include "locker.h"
#include <memory>
#include <stdexcept>

namespace clunk { namespace sdl {

Backend::Backend(int sample_rate, const u8 channels, int period_size) {
	LOG_DEBUG(("initializing backend %d %u %d", sample_rate, channels, period_size));
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
	src.callback = &Backend::callback;
	src.userdata = (void *) this;
	
	if ( SDL_OpenAudio(&src, &_spec) < 0 )
		throw_sdl(("SDL_OpenAudio(%d, %u, %d)", sample_rate, channels, period_size));
	if (_spec.format != AUDIO_S16SYS)
		throw_ex(("SDL_OpenAudio(%d, %u, %d) returned format %d", sample_rate, channels, period_size, _spec.format));
	if (_spec.channels < 2)
		LOG_ERROR(("Could not operate on %d channels", _spec.channels));

	LOG_DEBUG(("opened audio device, sample rate: %d, period: %d, channels: %d", _spec.freq, _spec.samples, _spec.channels));
	SDL_PauseAudio(0);
}

Backend::~Backend() {
	LOG_DEBUG(("DTOR"));
	if (!SDL_WasInit(SDL_INIT_AUDIO))
		return;

	AudioLocker l;
	SDL_PauseAudio(1);
	
	SDL_CloseAudio();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void Backend::callback(void *userdata, u8 *stream, int len) {
	Backend *self = static_cast<Backend *>(userdata);
	TRY {
		self->_context.process(stream, len);
	} CATCH("callback", {})
}


Sample* Backend::load(const std::string &file) {
	/*
	u8 *buf;
	u32 len;
	//SDL_AudioSpec * SDLCALL SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, u8 **audio_buf, u32 *audio_len);
	if (SDL_LoadWAV(file.c_str(), &_spec, &buf, &len) == NULL)
		throw_sdl(("SDL_LoadWav"));

	clunk::Buffer wav;
	wav.set_data(buf, len, true);
	_context->convert(_data, wav, _spec.freq, _spec.format, _spec.channels);
	
	name = file;
	 */
	throw std::runtime_error("port me");
}


}}
