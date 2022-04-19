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

#ifndef CLUNK_LOCKER_H__
#define CLUNK_LOCKER_H__

#include <SDL_audio.h>
#include <clunk/export_clunk.h>

#ifndef CLUNK_BACKEND_SDL
#	error invalid configuration
#endif


namespace clunk { namespace sdl {

/*! 
	\brief Audio callback locker
	This struct locks audio in ctor and releases lock from the dtor. 
	This prevents audio callback from being called while clunk::AudioLocker is in the scope. 
*/

struct CLUNKAPI AudioLocker {
	///locks audio 
	AudioLocker () {
		SDL_LockAudio();
	}
	///unlocks audio 
	~AudioLocker() {
		SDL_UnlockAudio();
	}
};
}}


#endif

