#ifndef __SDL_CXX_LAYER_EXCEPTION_H__
#define __SDL_CXX_LAYER_EXCEPTION_H__

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



#include "export_clunk.h"
#include "clunk_ex.h"

namespace clunk {
	class CLUNKAPI SDLException : public Exception {
	public: 
		virtual void add_custom_message();
	};
}

#define throw_sdl(s) throw_generic(clunk::SDLException, s);

#endif
