#ifndef MDCT_CONTEXT_H__
#define MDCT_CONTEXT_H__

#include "fft_context.h"
#include <string.h>

namespace clunk {

template<int BITS, typename T = float> 
class mdct_context {
private:
	typedef fft_context<BITS - 2, T> fft_type;
	fft_type fft;

public: 
	enum { N = 1 << BITS , M = N / 2, N4 =  fft_type::N };
	
	typedef std::complex<T> complex_type;

	T data[N];
	
	void mdct(bool inversion) {
		assert(N / 4 == N4); //we need static_assert :(
		if (!inversion) {
			T rotate[N];
			unsigned int t;
			for(t = 0; t < N4; ++t) {
				rotate[t] = -data[t + 3 * N4];
			}
			for(; t < N; ++t) {
				rotate[t] = data[t - N4];
			}
			for(t = 0; t < N4; ++t) {
				T re = (rotate[t * 2] - rotate[N - 1 - t * 2]) / 2;
				T im = (rotate[M + t * 2] - rotate[M - 1 - t * 2]) / -2;
				
				std::complex<T> a = std::polar<T>(1, 2 * T(M_PI) * (t + T(0.125)) / N);
				fft.data[t].real() = re * a.real() + im * a.imag();
				fft.data[t].imag() = -re * a.imag() + im * a.real();
			}
			fft.fft(false);
			T sqrt_N = sqrt(N);

			for(t = 0; t < N4; ++t) {
				std::complex<T> a = std::polar<T>(1, 2 * T(M_PI) * (t + T(0.125)) / N);
				std::complex<T>& f = fft.data[t];
				T f_real = f.real();
				f.real() = 2 / sqrt_N * (f_real * a.real() + f.imag() * a.imag());
				f.imag() = 2 / sqrt_N * (-f_real * a.imag() + f.imag() * a.real());
			}

			for(t = 0; t < N4; ++t) {
				data[2 * t] = fft.data[t].real();
				data[M - 2 * t - 1] = -fft.data[t].imag();
			}
		} else {
			unsigned int t; 
			for(t = 0; t < N4; ++t) {
				T re = data[t * 2] / 2, im = data[M - 1 - t * 2] / 2;
				std::complex<T> a = std::polar<T>(1, 2 * T(M_PI) * (t + T(0.125)) / N);
				fft.data[t].real() = re * a.real() + im * a.imag();
				fft.data[t].imag() = - re * a.imag() + im * a.real();
			}
			fft.fft(false);
			T sqrt_N = sqrt(N);
			
			for(t = 0; t < N4; ++t) {
				std::complex<T> a = std::polar<T>(1, 2 * T(M_PI) * (t + T(0.125)) / N);
				std::complex<T>& f = fft.data[t];
				T f_real = f.real();
				f.real() = 8 / sqrt_N * (f_real * a.real() + f.imag() * a.imag());
				f.imag() = 8 / sqrt_N * (-f_real * a.imag() + f.imag() * a.real());
			}

			T rotate[N];
			for(t = 0; t < N4; ++t) {
				rotate[2 * t] = fft.data[t].real();
				rotate[M + 2 * t] = fft.data[t].imag();
			}
			for(t = 1; t < N; t += 2) {
				rotate[t] = - rotate[N - t - 1];
			}

			//shift
			for(t = 0; t < 3 * N4; ++t) {
				data[t] = rotate[t + N4];
			}
			for(; t < N; ++t) {
				data[t] = -rotate[t - 3 * N4];
			}
		}
	}

	
	template<template<int, typename> class window_func>
	void apply() {
		window_func<N, T> func;
		for(unsigned i = 0; i < N; ++i) {
			data[i] *= func(i);
		}
	}
	
	void clear() {
		memset(data, 0, sizeof(data));
	}
	
private:
	inline T result(unsigned idx) const {
		int sign;
		if (idx & 1) {
			idx = N - idx - 1;
			sign = -1;
		} else 
			sign = 1;
		
		return sign * (
			(idx < N / 2)?
				fft.data[idx / 2].real():
				fft.data[(idx - N / 2) / 2].imag()
		);
	}
};

}

#endif
