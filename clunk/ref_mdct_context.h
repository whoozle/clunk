/*
MIT License

Copyright (c) 2008-2019 Netive Media Group & Vladimir Menshakov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CLUNK_REF_MDCT_CONTEXT_H__
#define CLUNK_REF_MDCT_CONTEXT_H__

#include <clunk/mdct_context.h>

namespace clunk {

template<int BITS, template <int, typename> class window_func_type , typename T = float>
class ref_mdct_context {
public:
	enum { N = 1 << BITS , M = N / 2, N2 = M, N4 = N / 4 };

private:
	const window_func_type<N, T> _window_func;
	T _cos[N][N2];
	T _sqrtN;

public:
	typedef T value_type;
	typedef std::complex<T> complex_type;

	T data[N];

	ref_mdct_context() : _window_func(), _sqrtN((T)sqrt((T)N)), data() {
		for(unsigned n = 0; n < N; ++n)
			for(unsigned k = 0; k < N2; ++k)
			{
				T a = M_PI * (n + 0.5 + N4) * (k + 0.5) / N2;
				_cos[n][k] = cos(a);
			}
	}
	
	void mdct()
	{
		T result[N2];
		for(unsigned k = 0; k < N2; ++k) {
			T v = 0;
			for(unsigned n = 0; n < N; ++n) {
				v += data[n] * _cos[n][k];
			}
			result[k] = v;
		}
		std::copy(result, result + N2, data);
	}
	
	void imdct() {
		T result[N];
		for(unsigned n = 0; n < N; ++n) {
			T v = 0;
			for(unsigned k = 0; k < N2; ++k) {
				v += data[k] * _cos[n][k];
			}
			result[n] = v / N4;
		}
		std::copy(result, result + N, data);
	}
	
	void apply_window() {
		for(unsigned i = 0; i < N; ++i) {
			data[i] *= _window_func.cache[i];
		}
	}
	
	void clear() {
		memset(data, 0, sizeof(data));
	}
};

}

#endif
