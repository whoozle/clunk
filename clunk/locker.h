#ifndef CLUNK_BACKEND_LOCKER_H
#define CLUNK_BACKEND_LOCKER_H

#ifdef CLUNK_BACKEND_SDL
#	include <clunk/backend/sdl/locker.h>
#endif

namespace clunk {
#ifdef CLUNK_BACKEND_SDL
	typedef sdl::AudioLocker AudioLocker;
#else
	struct AudioLocker {}; //dummy
#endif
}

#endif

