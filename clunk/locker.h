#ifndef CLUNK_BACKEND_LOCKER_H
#define CLUNK_BACKEND_LOCKER_H

#ifdef BACKEND_SDL
#	include <clunk/backend/sdl/locker.h>
#endif

namespace clunk {
#ifdef BACKEND_SDL
	typedef sdl::AudioLocker AudioLocker;
#else
	struct AudioLocker {}; //dummy
#endif
}

#endif

