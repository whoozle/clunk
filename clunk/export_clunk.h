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

#ifndef CLUNK_EXPORT_H__
#define CLUNK_EXPORT_H__

#include <clunk/config.h>

/* Shared library support */
#ifdef _MSC_VER
#	pragma warning(disable:4251)  /* needs to have dll-interface used by client */
#	pragma warning(disable:4275)  /* non dll-interface struct */
#	pragma warning(disable:4786) /* debug info exceedes 255 bytes */

#	define DLLIMPORT __declspec(dllimport)
#	define DLLEXPORT __declspec(dllexport)
#	define DLLDLLLOCAL
#	define DLLDLLPUBLIC
#else
#	ifdef GCC_HASCLASSVISIBILITY
#		define DLLIMPORT __attribute__ ((visibility("default")))
#		define DLLEXPORT __attribute__ ((visibility("default")))
#		define DLLDLLLOCAL __attribute__ ((visibility("hidden")))
#		define DLLDLLPUBLIC __attribute__ ((visibility("default")))
#	else
#		define DLLIMPORT
#		define DLLEXPORT
#		define DLLDLLLOCAL
#		define DLLDLLPUBLIC
#	endif
#endif

#ifndef CLUNKAPI
#	define CLUNKAPI DLLIMPORT
#endif

#ifndef CLUNKCAPI
#       ifdef __cplusplus
#               define CLUNKCAPI extern "C" CLUNKAPI
#       else
#               define CLUNKCAPI CLUNKAPI
#       endif
#endif

#endif
