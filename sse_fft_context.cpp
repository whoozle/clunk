#include <malloc.h>
#include <stdio.h>
#include <new>
#include "fft_context.h"
#include "sse_fft_context.h"

using namespace clunk;

void * aligned_allocator::allocate(size_t size, size_t alignment) {
	void * ptr;
#ifdef _WINDOWS
	ptr = _aligned_malloc(size, alignment);
	if (ptr == NULL)
		throw std::bad_alloc();
#else
	if (posix_memalign(&ptr, alignment, size) != 0) 
		throw std::bad_alloc();
	
#endif
	return ptr;
}

void aligned_allocator::deallocate(void *ptr) {
#ifdef _WINDOWS
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}
