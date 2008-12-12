#ifndef MDCT_CONTEXT_H__
#define MDCT_CONTEXT_H__

#include "fft_context.h"

namespace clunk {

template<int BITS, typename T = float> 
class mdct_context {
private:
	typedef fft_context<BITS - 2> fft_type;
	fft_type fft;

public: 
	enum { N = 1 << BITS , N4 =  fft_type::N };
	typedef std::complex<T> complex_type;

	T data[N];

	void mdct(bool inversion) {
		for(unsigned r = 0; r < N4; ++r) {
			complex_type v(data[2 * r], data[N / 2 + 2 * r]);
			complex_type e = std::polar<T>(1, 2 * (T)M_PI / N * (r + (T)0.125));
			fft.data[r] = v * e;
		}
		fft.fft(inversion);
		for(unsigned k = 0; k < N4; ++k) {
			complex_type e = std::polar<T>(2, 2 * (T)M_PI / N * (k + (T)0.125));
			fft.data[k] *= e;
		}
		
		for(unsigned i = 0; i < N; ++i) {
			data[i] = result(i);
		}
	}
	
	template<template<int, typename> class window_func>
	void apply() {
		window_func<N, T> func;
		for(unsigned i = 0; i < N; ++i) {
			data[i] *= func(i);
		}
	}

private:
	inline T result(unsigned idx) const {
		int sign;
		if (idx & 1) {
			idx = N - idx - 1;
			sign = -1;
		} else 
			sign = 1;
		
		return sign * ((idx >= N / 2)?fft.data[(idx - N / 2) / 2].imag(): fft.data[idx / 2].real());
	}
};

}

#endif
