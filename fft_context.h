#ifndef FFT_CONTEXT_H__
#define FFT_CONTEXT_H__

#include <math.h>
#include <complex>
#include <assert.h>

namespace clunk {

template<int N, typename T>
struct danielson_lanczos {
	enum { M = N / 2 };
	typedef danielson_lanczos<M, T> next_type;


	template<int SIGN>
	static void apply(std::complex<T>* data) {
		next_type::template apply<SIGN>(data);
		next_type::template apply<SIGN>(data + M);
		
		T a = (T)(-2 * M_PI / N * SIGN);
		T wtemp = sin(a / 2);
		
		std::complex<T> wp(-2 * wtemp * wtemp, sin(a)), w(1, 0);

		for (unsigned i = 0; i < M ; ++i) {
			int j = i + M;

			std::complex<T> temp = data[j] * w;

			data[j] = data[i] - temp;
			data[i] += temp;

			w += w * wp;
		}
	}
};

template<typename T>
struct danielson_lanczos<1, T> {
	template<int SIGN>
	static void apply(std::complex<T>*) {}
};


template<int BITS, typename T = float>
class fft_context {
public: 
	enum { N = 1 << BITS };
	typedef std::complex<T> value_type;
	value_type data[N];

public: 
	inline void fft() {
		scramble(data);
		next.template apply<1>(data);
	}

	inline void ifft() {
		scramble(data);
		next.template apply<-1>(data);
		for(unsigned i = 0; i < N; ++i) {
			data[i] /= N;
		}
	}
	
private:
	static void scramble(std::complex<T> * data) {
		int j = 0;
		for(int i = 0; i < N; ++i) {
			if (i > j) {
				std::swap(data[i], data[j]);
			}
			int m = N / 2;
			while(j >= m && m >= 2) {
				j -= m;
				m >>= 1;
			}
			j += m;
		}
	}

	danielson_lanczos<N, T> next;
	
};

}

#ifdef USE_SIMD
#	include "sse_fft_context.h"
#endif

#endif
