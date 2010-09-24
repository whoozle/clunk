project(clunk)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

find_package(SDL REQUIRED)
option(WITH_SSE "Use highly optimized SSE FFT/MDCT routines" false)

if ( NOT SDL_FOUND )
	message ( FATAL_ERROR "SDL not found!" )
endif ( NOT SDL_FOUND )

set(clunk_VERSION_MAJOR 1)
set(clunk_VERSION_MINOR 0)

add_definitions(-DCLUNKAPI=DLLEXPORT)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${SDL_INCLUDE_DIR})

set(SOURCES 
	buffer.cpp
	clunk_ex.cpp
	context.cpp
	distance_model.cpp
	fftw.c
	kemar.c
	logger.cpp
	object.cpp
	sample.cpp
	sdl_ex.cpp
	source.cpp
	stream.cpp
)
if (WITH_SSE)
	set(SOURCES ${SOURCES} sse_fft_context.cpp)
	add_definitions(-DCLUNK_USES_SSE)
endif(WITH_SSE)

add_library(clunk SHARED
	${SOURCES}
)

target_link_libraries(clunk ${SDL_LIBRARY})
