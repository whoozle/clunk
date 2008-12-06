#ifndef CLUNK_SIMD_HACKS_H__
#define CLUNK_SIMD_HACKS_H__

#ifdef USE_SIMD
#	include <xmmintrin.h>
#	define SIMD_LOAD(m, f) m = _mm_set_ss(f);
#	define SIMD_STORE(f, m) _mm_store_ss(&(f), m);
#	ifdef _MSC_VER
#		include <malloc.h>

		__forceinline __m128 operator*(__m128 a, __m128 b) {
			return _mm_mul_ps(a, b);
		}
		__forceinline __m128 operator+(__m128 a, __m128 b) {
			return _mm_add_ps(a, b);
		}
		__forceinline __m128 operator/(__m128 a, __m128 b) {
			return _mm_div_ps(a, b);
		}
		__forceinline __m128 operator-(__m128 a, __m128 b) {
			return _mm_sub_ps(a, b);
		}
		__forceinline __m128 operator-(__m128 a) {
			__m128 z = _mm_setzero_ps();
			return _mm_sub_ps(z, a);
		}
		__forceinline __m128& operator-=(__m128& a, __m128 b) {
			__m128 r = _mm_sub_ps(a, b);
			a = r;
			return a;
		}
		__forceinline __m128& operator+=(__m128& a, __m128 b) {
			__m128 r = _mm_add_ps(a, b);
			a = r;
			return a;
		}
		__forceinline __m128& operator*=(__m128& a, __m128 b) {
			__m128 r = _mm_mul_ps(a, b);
			a = r;
			return a;
		}

#	endif
#else
#	define	SIMD_LOAD(m, f)		m = (f)
#	define	SIMD_STORE(f, m)	f = (m)
#endif


#endif

