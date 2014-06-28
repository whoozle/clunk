#include <clunk/backend/sdl/backend.h>
#include <clunk/backend/sdl/sdl_ex.h>
#include <clunk/context.h>
#include <clunk/locker.h>
#include <SDL_rwops.h>
#include <SDL_audio.h>
#include <SDL.h>
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
	src.userdata = this;
	
	if ( SDL_OpenAudio(&src, &_spec) < 0 )
		throw_sdl(("SDL_OpenAudio(%d, %u, %d)", sample_rate, channels, period_size));
	if (_spec.format != AUDIO_S16SYS)
		throw_ex(("SDL_OpenAudio(%d, %u, %d) returned format %d", sample_rate, channels, period_size, _spec.format));
	if (_spec.channels < 2)
		LOG_ERROR(("Could not operate on %d channels", _spec.channels));

	LOG_DEBUG(("opened audio device, sample rate: %d, period: %d, channels: %d", _spec.freq, _spec.samples, _spec.channels));
	_context.init(convert(_spec));
}
void Backend::start()
{
	LOG_DEBUG(("starting output"));
	SDL_PauseAudio(0);
}

void Backend::stop()
{
	LOG_DEBUG(("stopping output"));
	SDL_PauseAudio(0);
}

Backend::~Backend() {
	LOG_DEBUG(("shutting down backend"));
	if (!SDL_WasInit(SDL_INIT_AUDIO))
		return;

	stop();

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
	Uint8 *buf;
	Uint32 len;
	SDL_AudioSpec spec;
	//SDL_AudioSpec * SDLCALL SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, u8 **audio_buf, u32 *audio_len);
	if (SDL_LoadWAV(file.c_str(), &spec, &buf, &len) == NULL)
		throw_sdl(("SDL_LoadWav"));
	clunk::Buffer wav;
	wav.set_data(buf, len, true);
	Sample *sample = _context.create_sample();
	sample->init(wav, convert(spec));
	sample->name = file;
	return sample;
}

SDL_AudioSpec Backend::convert(const AudioSpec &spec) {
	SDL_AudioSpec r = {};
	r.channels = spec.channels;
	r.freq = spec.sample_rate;
	switch(spec.format) {
		case AudioSpec::S8:		r.format = AUDIO_S8; break;
		case AudioSpec::U8:		r.format = AUDIO_U8; break;
		case AudioSpec::S16:	r.format = AUDIO_S16SYS; break;
		case AudioSpec::U16:	r.format = AUDIO_U16SYS; break;
		default: throw std::runtime_error("invalid audio format");
	}
	return r;
}

AudioSpec Backend::convert(const SDL_AudioSpec &spec) {
	AudioSpec::Format format;
	switch(spec.format) {
		case AUDIO_S8:		format = AudioSpec::S8; break;
		case AUDIO_U8:		format = AudioSpec::U8; break;
		case AUDIO_S16SYS:	format = AudioSpec::S16; break;
		case AUDIO_U16SYS:	format = AudioSpec::U16; break;
		default: throw std::runtime_error("invalid sdl audio format");
	}
	return AudioSpec(format, spec.freq, spec.channels);
}

}}
