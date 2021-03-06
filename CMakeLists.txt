project(clunk)
cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_USE_RELATIVE_PATHS TRUE)

option(BUILD_TEST "Build simple test application" false)
option(WITH_SSE "Use highly optimized SSE FFT/MDCT routines" false)
option(WITH_SDL "Use SDL backend" false)
option(WITH_SDL2 "Use SDL2 backend" true)

if (WITH_SDL2)
	message(STATUS "building with SDL2 support...")
	find_package(SDL2 REQUIRED)
elseif (WITH_SDL)
	message(STATUS "building with SDL1 support...")
	find_package(SDL REQUIRED)
endif()

set (CMAKE_CXX_STANDARD 11)
set(clunk_VERSION_MAJOR 1)
set(clunk_VERSION_MINOR 3)

message(STATUS "build type: ${CMAKE_BUILD_TYPE}")
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
	add_definitions(-DDEBUG)
endif (${CMAKE_BUILD_TYPE} MATCHES Debug)

if (CMAKE_CXX_COMPILER MATCHES ".*clang")
	set(CMAKE_COMPILER_IS_CLANGXX 1)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)
	add_definitions(-ggdb -Wall -pedantic -Wno-unused-variable)
	set(CMAKE_CXX_FLAGS "-fvisibility=hidden -fvisibility-inlines-hidden -DGCC_HASCLASSVISIBILITY=1")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGXX)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_WINDOWS)
endif(WIN32)

add_definitions(-DCLUNKAPI=DLLEXPORT)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
if (SDL_FOUND)
	include_directories(${SDL_INCLUDE_DIR})
endif()

if (SDL2_FOUND)
	include_directories(${SDL2_INCLUDE_DIRS})
endif()

set(SOURCES
	clunk/buffer.cpp
	clunk/clunk_ex.cpp
	clunk/context.cpp
	clunk/distance_model.cpp
	clunk/hrtf.cpp
	clunk/kemar.c
	clunk/logger.cpp
	clunk/object.cpp
	clunk/sample.cpp
	clunk/source.cpp
	clunk/stream.cpp
	clunk/wav_file.cpp
#	clunk/clunk_c.cpp
)

if (SDL_FOUND OR SDL2_FOUND)
	set(CLUNK_BACKEND_SDL 1)
	list(APPEND SOURCES
		clunk/backend/sdl/sdl_ex.cpp
		clunk/backend/sdl/backend.cpp
	)
endif ()

set(PUBLIC_HEADERS
	clunk/buffer.h
	clunk/clunk.h
	clunk/clunk_assert.h
	clunk/context.h
	clunk/distance_model.h
	clunk/export_clunk.h
	clunk/fft_context.h
	clunk/hrtf.h
	clunk/kemar.h
	clunk/locker.h
	clunk/logger.h
	clunk/mdct_context.h
	clunk/object.h
	clunk/ref_mdct_context.h
	clunk/sample.h
	clunk/source.h
	clunk/sse_fft_context.h
	clunk/stream.h
	clunk/v3.h
	clunk/clunk_c.h
	clunk/window_function.h
	${CMAKE_CURRENT_BINARY_DIR}/clunk/config.h
)

if (SDL_FOUND OR SDL2_FOUND)
	list(APPEND PUBLIC_HEADERS
		clunk/backend/sdl/backend.h
	)
endif ()

if (WITH_SSE)
	set(SOURCES ${SOURCES} clunk/sse_fft_context.cpp)
	set(CLUNK_USES_SSE 1)
endif(WITH_SSE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/clunk/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/clunk/config.h)

add_library(clunk SHARED
	${SOURCES}
)

target_include_directories(clunk PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(clunk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(clunk-static STATIC
	${SOURCES}
)
target_include_directories(clunk-static PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(clunk-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS clunk DESTINATION lib)
install(FILES ${PUBLIC_HEADERS} DESTINATION include/clunk)

if (SDL_FOUND)
    target_link_libraries(clunk ${SDL_LIBRARY})
endif (SDL_FOUND)

if (SDL2_FOUND)
    target_link_libraries(clunk ${SDL2_LIBRARIES})
endif (SDL2_FOUND)

if(BUILD_TEST)
	if (NOT SDL_FOUND AND NOT SDL2_FOUND)
		message(FATAL_ERROR "you need SDL library to build test")
	endif ()

	add_executable(clunk-test test.cpp)
	target_link_libraries(clunk-test clunk)

	add_executable(clunk-mdct clunk-mdct.cpp)
	target_link_libraries(clunk-mdct clunk)
endif(BUILD_TEST)
