#ifndef CLUNK_BACKEND_LOCKER_H
#define CLUNK_BACKEND_LOCKER_H

#include "backend/sdl/locker.h"

namespace clunk {
#ifdef BACKEND_SDL
	typedef sdl::AudioLocker AudioLocker;
#else
#	error you need audiolocker object to avoid races
#endif
}

#endif

