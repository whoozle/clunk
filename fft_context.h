#ifndef FFT_CONTEXT_H__
#define FFT_CONTEXT_H__

#include <math.h>
#include <complex>
#include <assert.h>

namespace clunk {

template<int N, typename T>
class danielson_lanczos {
public:
	enum { M = N / 2 };
	danielson_lanczos<M, T> next;

	void apply(std::complex<T>* data, bool inversion) {
		next.apply(data, inversion);
		next.apply(data + M, inversion);
		int sign = inversion? -1: 1;
		
		for(unsigned i = 0; i < M; ++i) {
			std::complex<T> t = data[i + M] * std::polar<T>(sign, i * (T)M_PI / M); //optimize it
			data[i + M] = data[i] - t;
			data[i] += t;
		}
	};
};

template<typename T>
class danielson_lanczos<1, T> {
public:
	void apply(std::complex<T>*, bool) {}
};


template<int BITS, typename T = float>
class fft_context {
public: 
	enum { N = 1 << BITS };
	typedef std::complex<T> value_type;
	value_type data[N];
	
	inline void fft(bool inversion) {
		scramble();
		next.apply(data, inversion);
		for(unsigned i = 0; i < N; ++i) {
			data[i] /= (inversion?-1: 1) * sqrt(N);
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
		unsigned j = 1;
		for (unsigned i = 1; i < 2 * N; i += 2) {
			assert(j < 2 * N);
			if (j > i) {
				//assert(j & 1);
				//assert(i & 1);
				//printf("%x <-> %x\n", (i - 1) / 2, (j - 1) / 2);
				swap(data[(j - 1) / 2], data[(i - 1) / 2]);
			}
			unsigned m = N;
			while (m >= 2 && j > m) {
				j -= m;
				m >>= 1;
			}
			j += m;
		}
	}
};

}

#endif
