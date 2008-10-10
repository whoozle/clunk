/* libclunk - realtime 2d/3d sound render library
 * Copyright (C) 2007-2008 Netive Media Group
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/


#define _USE_MATH_DEFINES
#include <math.h>
#include "source.h"
#include <SDL.h>
#include "clunk_ex.h"
#include "buffer.h"
#include "sample.h"
#include <assert.h>

#if defined _WINDOWS || __APPLE__
#	define pow10f(x) powf(10.0f, (x))
#	define log2f(x) (logf(x) / M_LN2)
#endif

#include "kiss/clunk_simd_hacks.h"


template <typename T> inline T clunk_min(T a, T b) {
	return a < b? a: b;
}

template <typename T> inline T clunk_max(T a, T b) {
	return a > b? a: b;
}

using namespace clunk;
Source::Source(const Sample * sample, const bool loop, const v3<float> &delta, float gain, float pitch) : 
	sample(sample), loop(loop), delta_position(delta), gain(gain), pitch(pitch), 
	position(0), fadeout(0), fadeout_total(0),
	fft_state(NULL), ffti_state(NULL) 
	{
	for(int i = 0; i < 2; ++i) {
		use_overlap[i] = false;
	}
	
	if (sample == NULL)
		throw_ex(("sample for source cannot be NULL"));
}
	
bool Source::playing() const {
	if (fadeout_total > 0 && fadeout <= 0)
		return false;
	
	return loop?true: position < (int)(sample->data.get_size() / sample->spec.channels / 2);
}
	
void Source::idt(const v3<float> &delta, const v3<float> &dir_vec, float &idt_offset, float &angle_gr) {
	float head_r = 0.09554140127388535032f;

	float direction = dir_vec.is0()? M_PI_2: atan2f(dir_vec.y, dir_vec.x);
	float angle = direction - atan2f(delta.y, delta.x);
	
	angle_gr = angle * 180 / M_PI;
	while (angle_gr < 0)
		angle_gr += 360;
	
	//LOG_DEBUG(("relative position = (%g,%g,%g), angle = %g (%g)", delta.x, delta.y, delta.z, angle, angle_gr));
	
	float idt_angle = fmodf(angle, 2 * (float)M_PI);
	if (idt_angle < 0)
		idt_angle += 2 * (float)M_PI;

	if (idt_angle >= M_PI_2 && idt_angle < M_PI) {
		idt_angle = M_PI - idt_angle;
	} else if (idt_angle >= M_PI && idt_angle < 3 * M_PI_2) {
		idt_angle = M_PI - idt_angle;
	} else if (idt_angle >= 3 * M_PI_2) {
		idt_angle -= M_PI * 2;
	}

	//printf("idt_angle = %g (%d)\n", idt_angle, (int)(idt_angle * 180 / M_PI));
	idt_offset = - head_r / 343 * (idt_angle + sin(idt_angle));
	//printf("idt_offset %g", idt_offset);
}

#include "kiss/kiss_fftr.h"

#define CLUNK_ACTUAL_WINDOW (CLUNK_WINDOW_SIZE - CLUNK_WINDOW_OVERLAP)

void Source::hrtf(const unsigned channel_idx, clunk::Buffer &result, int dst_n, const Sint16 *src, int src_ch, int src_n, int idt_offset, const kemar_ptr& kemar_data, int kemar_idx) {
	//const int lowpass_cutoff = 5000 * CLUNK_ACTUAL_WINDOW / sample->spec.freq;
	//LOG_DEBUG(("using cutoff at %d", lowpass_cutoff));
	assert(channel_idx < 2);
	if (fft_state == NULL)
		fft_state = kiss_fftr_alloc(CLUNK_WINDOW_SIZE, 0, NULL, NULL);
	if (ffti_state == NULL)
		ffti_state = kiss_fftr_alloc(CLUNK_WINDOW_SIZE, 1, NULL, NULL);
	
	int n = (dst_n - 1) / CLUNK_ACTUAL_WINDOW + 1;
	//LOG_DEBUG(("%d bytes, %d actual window size, %d windows", dst_n, CLUNK_ACTUAL_WINDOW, n));
	result.set_size(2 * dst_n);

	Sint16 *dst = (Sint16 *)result.get_ptr();

	if (channel_idx <= 1) {
		bool left = channel_idx == 0;
		if (!left && idt_offset > 0) {
			idt_offset = 0;
		} else if (left && idt_offset < 0) {
			idt_offset = 0;
		}
		if (idt_offset < 0)
			idt_offset = - idt_offset;
	} else 
		idt_offset = 0;
	
	for(int i = 0; i < n; ++i) {
		kiss_fft_scalar src_data[CLUNK_WINDOW_SIZE];
		kiss_fft_cpx freq[CLUNK_WINDOW_SIZE / 2 + 1];
		//printf("fft #%d\n", i);
		for(int j = 0; j < CLUNK_WINDOW_SIZE; ++j) {
			int idx = i * CLUNK_ACTUAL_WINDOW + j;
			int p = (int)(position + idx * pitch + idt_offset);

			int v = 0;
			if (fadeout_total > 0 && fadeout - idx <= 0) {
				//no sound
			} else if (p >= 0 || p < src_n || loop) {
				p %= src_n;
				if (p < 0)
					p += src_n;
				v = src[p * src_ch];
			}
			if (fadeout_total > 0 && fadeout - idx > 0) {
				//LOG_DEBUG(("fadeout %d: %d -> %d", fadeout - idx, v, v * (fadeout - idx) / fadeout_total));
				v = v * (fadeout - idx) / fadeout_total;
			}
			float vv = (v / 32767.0f); // * sin(M_PI * j / (CLUNK_WINDOW_SIZE - 1));
			SIMD_LOAD(src_data[j], vv);
		}
		
		kiss_fftr(fft_state, src_data, freq);
		
		//LOG_DEBUG(("kemar angle index: %d\n", kemar_idx));
		for(int j = 0; j <= CLUNK_WINDOW_SIZE / 2; ++j) {
			//float * dst = (ch == 0)?tr_left + pos:tr_right + pos;
			kiss_fft_scalar mlen = freq[j].r * freq[j].r + freq[j].i * freq[j].i;
			float len;
			SIMD_STORE(len, mlen);
			len = sqrt(len);
			//LOG_DEBUG(("length: %g", len));
			const int kemar_angle_idx = j * 512 / (CLUNK_WINDOW_SIZE / 2 + 1);
			assert(kemar_angle_idx < 512);
			float m = pow10f(kemar_data[kemar_idx][1][kemar_angle_idx] * len / 20);
			/*
			if (j > lowpass_cutoff) {
				float k = 1.0f - (1.0f * j - lowpass_cutoff) / (CLUNK_WINDOW_SIZE / 2 - lowpass_cutoff);
				m *= k / 2;
				//LOG_DEBUG(("%d -> %g", j, k);
			}
			*/
			kiss_fft_scalar mm;
			SIMD_LOAD(mm, m);
			freq[j].r = freq[j].r * mm; //do not replace with *=, breaks compilation on windows
			freq[j].i = freq[j].r * mm;
			//float len2 = sqrt(freq[j].r * freq[j].r + freq[j].i * freq[j].i);
			//LOG_DEBUG(("%d: multiplicator = %g, len: %g -> %g", j, m, len, len2));
		}

		kiss_fftri(ffti_state, freq, src_data);
		
		int offset = i * CLUNK_ACTUAL_WINDOW;
		int more = dst_n - offset;

		float max = CLUNK_WINDOW_SIZE;
		int jmax = clunk_min(more, CLUNK_ACTUAL_WINDOW);
		int jmin = clunk_min(jmax, CLUNK_WINDOW_OVERLAP);
		//LOG_DEBUG(("last chunk : %d, overlap first %d, more: %d", jmax, jmin, more));

		for(int j = 0; j < jmax + CLUNK_WINDOW_OVERLAP; ++j) {
			float v;
			SIMD_STORE(v, src_data[j]);
			if (v > max) {
				//LOG_DEBUG(("increased max to %g", v));
				max = v;
			} else if (v < -max) {
				//LOG_DEBUG(("increased min to %g", v));
				max = -v;
			}
			int x = (int)(v / max * 32766);
			//if (x > 32767 || x < -32767) 
			//	LOG_WARN(("sample overflow: %d", x));
			
			if (j >= jmax) {
				assert(j - jmax < CLUNK_WINDOW_OVERLAP);
				overlap_data[channel_idx][j - jmax] = x;
				//if (jmax != CLUNK_ACTUAL_WINDOW)
				//	LOG_DEBUG(("overlap[%d] = %d", j - jmax, x));
				use_overlap[channel_idx] = true;
			} else {
				assert(offset + j < dst_n);

				if (use_overlap[channel_idx] && j < jmin) {
					x = (x * j + overlap_data[channel_idx][j] * (jmin - j)) / jmin;
				}

				dst[offset + j] = x;
			}
		}
		//assert (jmax == CLUNK_ACTUAL_WINDOW || offset + jmax == dst_n);
	}
}

void Source::update_position(const int dp) {
	int src_n = sample->data.get_size() / sample->spec.channels / 2;
	position += dp;
	if (loop) {
		position %= src_n;
		//LOG_DEBUG(("position %d", position));
		if (position < 0)
			position += src_n;
	}
	if (fadeout_total > 0) {
		fadeout -= dp;
		if (fadeout <= 0) {
			fadeout = 0;
			loop = false;
		}
	}
}

float Source::process(clunk::Buffer &buffer, unsigned dst_ch, const v3<float> &delta_position, const v3<float> &direction, float fx_volume, float pitch) {
	Sint16 * dst = (Sint16*) buffer.get_ptr();
	unsigned dst_n = buffer.get_size() / dst_ch / 2;
	const Sint16 * src = (Sint16*) sample->data.get_ptr();
	if (src == NULL)
		throw_ex(("uninitialized sample used (%p)", (void *)sample));

	pitch *= this->pitch;
	if (pitch <= 0)
		throw_ex(("pitch %g could not be negative or zero", pitch));
		
	unsigned src_ch = sample->spec.channels; 
	unsigned src_n = sample->data.get_size() / src_ch / 2;

	float vol = fx_volume * gain;
	
	if (vol > 1)
		vol = 1;

	if (vol < 0 || (int)floor(SDL_MIX_MAXVOLUME * vol + 0.5f) <= 0) {
		update_position((int)(dst_n * pitch));
		return 0;
	}
	
	kemar_ptr kemar_data;
	int angles;
	get_kemar_data(kemar_data, angles, delta_position);

	if (delta_position.is0() || kemar_data == NULL) {
		//2d stereo sound! 
		for(unsigned i = 0; i < dst_n; ++i) {
			for(unsigned c = 0; c < dst_ch; ++c) {
				int p = position + (int)(i * pitch);
			
				Sint16 v = 0;
				if (p >= 0 && p < (int)src_n) {
					if (c < src_ch) {
						v = src[p * src_ch + c];
					} else {
						v = src[p * src_ch];//expand mono channel if needed
					}
				}
				dst[i * dst_ch + c] = v;
			}
		}
		update_position((int)(dst_n * pitch));
		return vol;
	}
	
	//LOG_DEBUG(("data: %p, angles: %d", (void *) kemar_data, angles));
	update_position(0);
	
	if (position >= (int)src_n) {
		//LOG_WARN(("process called on inactive source"));
		return 0;
	}

	float t_idt, angle_gr;
	idt(delta_position, direction, t_idt, angle_gr);

	const int kemar_idx_right = ((((int)angle_gr  + 180 / (int)angles)/ (360 / (int)angles)) % (int)angles);
	const int kemar_idx_left = (((360 - (int)angle_gr - 180 / (int)angles) / (360 / (int)angles)) % (int)angles);
	
	int idt_offset = (int)(t_idt * sample->spec.freq);

	clunk::Buffer sample3d_left, sample3d_right;

	hrtf(0, sample3d_left, dst_n, src, src_ch, src_n, idt_offset, kemar_data, kemar_idx_left);
	hrtf(1, sample3d_right, dst_n, src, src_ch, src_n, idt_offset, kemar_data, kemar_idx_right);
	
	//LOG_DEBUG(("angle: %g", angle_gr));
	//LOG_DEBUG(("idt offset %d samples", idt_offset));
	Sint16 * src_3d[2] = { (Sint16 *)sample3d_left.get_ptr(), (Sint16 *)sample3d_right.get_ptr() };
	
	//LOG_DEBUG(("size: %u", (unsigned)sample3d_left.get_size()));
	
	for(unsigned i = 0; i < dst_n; ++i) {
		for(unsigned c = 0; c < dst_ch; ++c) {
			dst[i * dst_ch + c] = src_3d[c][i];
		}
	}
	update_position((int)(dst_n * pitch));
	return vol;
}

#include "kemar.h"

void Source::get_kemar_data(kemar_ptr & kemar_data, int & elev_n, const v3<float> &pos) {
	
	kemar_data = NULL;
	elev_n = 0;
	if (pos.is0())
		return;

	int elev_gr = (int)(180 * atan2f(pos.z, sqrt(pos.x * pos.x + pos.y * pos.y)) / M_PI);

	if (elev_gr < -35) {
		kemar_data = elev_m40;
		elev_n = ELEV_M40_N;
	} else if (elev_gr < -25) {
		kemar_data = elev_m30;
		elev_n = ELEV_M30_N;
	} else if (elev_gr < -15) {
		kemar_data = elev_m20;
		elev_n = ELEV_M20_N;
	} else if (elev_gr < -5) {
		kemar_data = elev_m10;
		elev_n = ELEV_M10_N;
	} else if (elev_gr < 5) {
		kemar_data = elev_0;
		elev_n = ELEV_0_N;
	} else if (elev_gr < 15) {
		kemar_data = elev_10;
		elev_n = ELEV_10_N;
	} else if (elev_gr < 25) {
		kemar_data = elev_20;
		elev_n = ELEV_20_N;
	} else if (elev_gr < 35) {
		kemar_data = elev_30;
		elev_n = ELEV_30_N;
	} else if (elev_gr < 45) {
		kemar_data = elev_40;
		elev_n = ELEV_40_N;
	} else if (elev_gr < 55) {
		kemar_data = elev_50;
		elev_n = ELEV_50_N;
	} else if (elev_gr < 65) {
		kemar_data = elev_60;
		elev_n = ELEV_60_N;
	} else if (elev_gr < 75) {
		kemar_data = elev_70;
		elev_n = ELEV_70_N;
	} else if (elev_gr < 85) {
		kemar_data = elev_80;
		elev_n = ELEV_80_N;
	} else {
		kemar_data = elev_90;
		elev_n = ELEV_90_N;
	}
}

Source::~Source() {
	if (fft_state != NULL) 
		kiss_fft_free(fft_state);
	if (ffti_state != NULL)
		kiss_fft_free(ffti_state);
}

void Source::fade_out(const float sec) {
	fadeout = fadeout_total = (int)(sample->spec.freq * sec);
}
