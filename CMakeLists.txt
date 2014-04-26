project(clunk)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

find_package(SDL REQUIRED)
option(WITH_SSE "Use highly optimized SSE FFT/MDCT routines" false)

set(clunk_VERSION_MAJOR 1)
set(clunk_VERSION_MINOR 1)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DDEBUG)
	if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
		add_definitions(-ggdb -Wall -pedantic)
	endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WINDOWS)
endif(WIN32)

add_definitions(-DCLUNKAPI=DLLEXPORT)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${SDL_INCLUDE_DIR})

set(SOURCES 
	clunk/buffer.cpp
	clunk/clunk_ex.cpp
	clunk/context.cpp
	clunk/distance_model.cpp
	clunk/kemar.c
	clunk/logger.cpp
	clunk/object.cpp
	clunk/sample.cpp
	clunk/source.cpp
	clunk/stream.cpp
	clunk/wav_file.cpp
#	clunk/clunk_c.cpp
)

if (SDL_FOUND)
	add_definitions(-DBACKEND_SDL=1)
	list(APPEND SOURCES
		clunk/backend/sdl/sdl_ex.cpp
		clunk/backend/sdl/backend.cpp
	)
endif (SDL_FOUND)

set(PUBLIC_HEADERS
	clunk/buffer.h
	clunk/clunk.h
	clunk/clunk_assert.h
	clunk/context.h
	clunk/distance_model.h
	clunk/export_clunk.h
	clunk/fft_context.h
	clunk/locker.h
	clunk/logger.h
	clunk/mdct_context.h
	clunk/object.h
	clunk/sample.h
	clunk/source.h
	clunk/sse_fft_context.h
	clunk/stream.h
	clunk/v3.h
	clunk/clunk_c.h
)

if (SDL_FOUND)
	list(APPEND PUBLIC_HEADERS
		clunk/backend/sdl/backend.h
	)
endif (SDL_FOUND)

if (WITH_SSE)
	set(SOURCES ${SOURCES} sse_fft_context.cpp)
	add_definitions(-DCLUNK_USES_SSE)
endif(WITH_SSE)

add_library(clunk SHARED
	${SOURCES}
)

install(TARGETS clunk DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/clunk)

target_link_libraries(clunk ${SDL_LIBRARY})
if(BUILD_TEST)
	add_executable(clunk-test test.cpp)
	target_link_libraries(clunk-test clunk)
endif(BUILD_TEST)
