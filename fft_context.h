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

	static void apply(std::complex<T>* data, bool inversion) {
		next_type::apply(data, inversion);
		next_type::apply(data + M, inversion);
			
		int sign = inversion? -1: 1;
		T a = (T)(-2 * M_PI / N * sign);
		T wtemp = sin(a / 2);
		
		std::complex<T> wp(-2 * wtemp * wtemp, sin(a)), w(1, 0);

		for (unsigned i = 0; i < M ; ++i) {
			int j = i + M;

			std::complex<T> temp = data[j] * w;

			data[j] = data[i] - temp;
			data[i] += temp;

			w += w * wp;
		}
	};
};

template<typename T>
struct danielson_lanczos<1, T> {
	static void apply(std::complex<T>*, bool) {}
};


template<int BITS, typename T = float>
class fft_context {
public: 
	enum { N = 1 << BITS };
	typedef std::complex<T> value_type;
	value_type data[N];
	
	inline void fft() {
		scramble();
		next.apply(data, false);
	}

	inline void ifft() {
		scramble();
		next.apply(data, true);
		for(unsigned i = 0; i < N; ++i) {
			data[i] /= N;
		}
	}
	
private:
	danielson_lanczos<N, T> next;
	
	template<typename V>
	static inline void swap(V &a, V &b) {
		V t = a;
		a = b; 
		b = t;
	}

	void scramble() {
		int j = 0;
		for(int i = 0; i < N; ++i) {
			if (i > j) {
				swap(data[i], data[j]);
			}
			int m = N / 2;
			while(j >= m && m >= 2) {
				j -= m;
				m >>= 1;
			}
			j += m;
		}
	}
};

}

#endif
