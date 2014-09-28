#ifndef CLUNK_HRTF_H
#define	CLUNK_HRTF_H

#include <clunk/buffer.h>
#include <clunk/export_clunk.h>
#include <clunk/mdct_context.h>
#include <clunk/types.h>
#include <clunk/v3.h>

namespace clunk {

//window function used in ogg/vorbis
template<int N, typename T>
struct vorbis_window_func : public clunk::window_func_base<N, T, vorbis_window_func<N, T> > {
	inline T operator()(int x) const {
		T s = sin(T(M_PI) * (x + (T)0.5) / N);
		return sin(T(M_PI_2) * s * s); 
	}
};

template<int N, typename T>
struct sin_window_func : public clunk::window_func_base<N, T, sin_window_func< N, T> > {
	inline T operator()(int x) const {
		return sin(T(M_PI) * (x + (T)0.5) / N);
	}
};

template<int N, typename T>
struct kbd_window_func {
	T cache[N];

	static T BesselI0(T x) {
		T denominator;
		T numerator;
		T z;

		if (x == 0.0) {
		   return 1.0;
		} else {
		   z = x * x;
		   numerator = (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z* (z*
						  (z* 0.210580722890567e-22  + 0.380715242345326e-19 ) +
							  0.479440257548300e-16) + 0.435125971262668e-13 ) +
							  0.300931127112960e-10) + 0.160224679395361e-7  ) +
							  0.654858370096785e-5)  + 0.202591084143397e-2  ) +
							  0.463076284721000e0)   + 0.754337328948189e2   ) +
							  0.830792541809429e4)   + 0.571661130563785e6   ) +
							  0.216415572361227e8)   + 0.356644482244025e9   ) +
							  0.144048298227235e10);

		   denominator = (z*(z*(z-0.307646912682801e4)+
							0.347626332405882e7)-0.144048298227235e10);
		}

		return -numerator/denominator;
	}

	kbd_window_func(int alpha = 4) {
		T sumvalue = 0.0;
		for (int i=0; i< N / 2; ++i) {
		   sumvalue += BesselI0(M_PI * alpha * sqrt(1.0 - pow(4.0 * i / N - 1.0, 2)));
		   cache[i] = sumvalue;
		}

		// need to add one more value to the nomalization factor at size/2:
		sumvalue += BesselI0(M_PI * alpha * sqrt(1.0 - pow(4.0 * (N/2) / N - 1.0, 2)));

		// normalize the window and fill in the righthand side of the window:
		for (int i=0; i< N/2; ++i) {
		   cache[i] = sqrt(cache[i] / sumvalue);
		   cache[N - 1 - i] = cache[i];
		}
	}
};

class CLUNKAPI Hrtf {
public: 
	enum { WINDOW_BITS = 9 };

private: 
	typedef mdct_context<WINDOW_BITS, vorbis_window_func, float> mdct_type;
	mdct_type _mdct;

public:
	enum { WINDOW_SIZE = mdct_type::N };

	Hrtf();

	void process(unsigned sample_rate, clunk::Buffer &dst_buf, unsigned dst_ch,
			const clunk::Buffer &src_buf, unsigned src_ch,
			const v3f &position, float fx_volume);

	void skip(unsigned samples);

private:
	static void idt_iit(const v3f &position, float &idt_offset, float &angle_gr, float &left_to_right_amp);
	typedef const float (*kemar_ptr)[2][257][2];
	void get_kemar_data(kemar_ptr & kemar_data, int & samples, const v3f &delta_position);

	//generate hrtf response for channel idx (0 left), in result.
	void hrtf(int window, const unsigned channel_idx, clunk::Buffer &result, const s16 *src, int src_ch, int src_n, int idt_offset, const kemar_ptr& kemar_data, int kemar_idx, float freq_decay);

private:
	clunk::Buffer sample3d[2];
	float overlap_data[2][WINDOW_SIZE / 2];
};

}

#endif
