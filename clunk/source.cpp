/* libClunk - cross-platform 3D audio API built on top SDL library
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


#include <clunk/source.h>
#include <clunk/clunk_ex.h>
#include <clunk/buffer.h>
#include <clunk/sample.h>
#include <assert.h>
#include <clunk/clunk_assert.h>
#include <clunk/mixer.h>

#if defined _MSC_VER || __APPLE__ || __FreeBSD__
#	define pow10f(x) powf(10.0f, (x))
#	define log2f(x) (logf(x) / M_LN2)
#endif

using namespace clunk;

Source::mdct_type Source::mdct __attribute__ ((aligned (16)));

clunk_static_assert(Source::WINDOW_BITS > 2);

template <typename T> inline T clunk_min(T a, T b) {
	return a < b? a: b;
}

template <typename T> inline T clunk_max(T a, T b) {
	return a > b? a: b;
}

Source::Source(const Sample * sample, const bool loop, const v3f &delta, float gain, float pitch, float panning):
	sample(sample), loop(loop), delta_position(delta), gain(gain), pitch(pitch), panning(panning), 
	position(0), fadeout(0), fadeout_total(0)
	{
	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < WINDOW_SIZE / 2; ++j) {
			overlap_data[i][j] = 0;
		}
	}
	
	if (sample == NULL)
		throw_ex(("sample for source cannot be NULL"));
}
	
bool Source::playing() const {
	if (fadeout_total > 0 && fadeout <= 0)
		return false;

	if (loop) 
		return true;
	
	//if (!sample3d[0].empty() || !sample3d[1].empty())
	//	return true;
	
	return position < (int)(sample->_data.get_size() / sample->_spec.channels / 2);
}
	
void Source::idt_iit(const v3f &position, float &idt_offset, float &angle_gr, float &left_to_right_amp) {
	float head_r = 0.093f;

	float angle = M_PI_2 - atan2f(position.y, position.x);
	
	angle_gr = angle * 180 / float(M_PI);
	while (angle_gr < 0)
		angle_gr += 360;
	
	//LOG_DEBUG(("relative position = (%g,%g,%g), angle = %g (%g)", position.x, position.y, position.z, angle, angle_gr));
	
	float idt_angle = fmodf(angle, 2 * (float)M_PI);

	if (idt_angle < 0)
		idt_angle += 2 * (float)M_PI;
	if (idt_angle >= float(M_PI_2) && idt_angle < (float)M_PI) {
		idt_angle = float(M_PI) - idt_angle;
	} else if (idt_angle >= float(M_PI) && idt_angle < 3 * float(M_PI_2)) {
		idt_angle = (float)M_PI - idt_angle;
	} else if (idt_angle >= 3 * (float)M_PI_2) {
		idt_angle -= (float)M_PI * 2;
	}

	//LOG_DEBUG(("idt_angle = %g (%d)", idt_angle, (int)(idt_angle * 180 / M_PI)));
	idt_offset = - head_r * (idt_angle + sin(idt_angle)) / 344;
	left_to_right_amp = pow10f(-sin(idt_angle));
	//LOG_DEBUG(("idt_offset %g, left_to_right_amp: %g", idt_offset, left_to_right_amp));
}

void Source::hrtf(int window, const unsigned channel_idx, clunk::Buffer &result, const s16 *src, int src_ch, int src_n, int idt_offset, const kemar_ptr& kemar_data, int kemar_idx, float freq_decay) {
	assert(channel_idx < 2);
	
	//LOG_DEBUG(("%d bytes, %d actual window size, %d windows", dst_n, CLUNK_ACTUAL_WINDOW, n));
	//result.set_size(2 * WINDOW_SIZE / 2); //sizeof(s16) * window  / 2
	size_t result_start = result.get_size();
	result.reserve(WINDOW_SIZE);
	
	//LOG_DEBUG(("channel %d: window %d: adding %d, buffer size: %u, decay: %g", channel_idx, window, WINDOW_SIZE, (unsigned)result.get_size(), freq_decay));

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

	for(int i = 0; i < WINDOW_SIZE; ++i) {
		//-1 0 1 2 3
		int p = position + idt_offset + (int)((window * WINDOW_SIZE / 2 + i) * pitch); //overlapping half
		//printf("%d of %d, ", p, src_n);
		int v = 0;
		if (fadeout_total > 0 && fadeout - i <= 0) {
			//v = 0;
		} else if (loop || (p >= 0 && p < src_n)) {
			p %= src_n;
			if (p < 0)
				p += src_n;
			v = src[p * src_ch];
		}
		//assert(v < 32768 && v > -32768);
		if (fadeout_total > 0 && fadeout - i > 0) {
			//LOG_DEBUG(("fadeout %d: %d -> %d", fadeout - i, v, v * (fadeout - i) / fadeout_total));
			v *= (fadeout - i) / fadeout_total;
		}
		mdct.data[i] = v / 32768.0f;
		//fprintf(stderr, "%g ", mdct.data[i]);
	}
	
	mdct.apply_window();
	typedef fft_context<WINDOW_BITS, float> filter_type;
	filter_type fir;
	for(size_t i = 0; i < filter_type::N; ++i) {
		const int kemar_sample = i * 512 / filter_type::N;
		fir.data[i] = std::complex<float>(kemar_data[kemar_idx][0][kemar_sample] / 32768.0f, 0);
	}
	fir.fft();
	mdct.mdct();
	{
		float e1 = 0, e2 = 0;
		for(size_t i = 0; i < mdct_type::M; ++i)
		{
			e1 += mdct.data[i] * mdct.data[i];
			mdct.data[i] *= std::abs(fir.data[i * mdct_type::M / filter_type::N]);
			e2 += mdct.data[i] * mdct.data[i];
		}

		float norm = sqrt(e1 / e2);
		for(size_t i = 0; i < mdct_type::M; ++i)
		{
			mdct.data[i] *= norm;
		}
	}

	//LOG_DEBUG(("kemar angle index: %d\n", kemar_idx));
	assert(freq_decay >= 1);
	
	mdct.imdct();
	mdct.apply_window();

	s16 *dst = static_cast<s16 *>(static_cast<void *>((static_cast<u8 *>(result.get_ptr()) + result_start)));

	float max_v = 1.0f, min_v = -1.0f;
	
	for(int i = 0; i < WINDOW_SIZE / 2; ++i) {
		float v = (mdct.data[i] + overlap_data[channel_idx][i]);

		if (v < min_v)
			min_v = v;
		else if (v > max_v)
			max_v = v;
	}

	{
		//stupid msvc
		int i;
		for(i = 0; i < WINDOW_SIZE / 2; ++i) {
			float v = ((mdct.data[i] + overlap_data[channel_idx][i]) - min_v) / (max_v - min_v) * 2 - 1;
			
			if (v < -1) {
				LOG_DEBUG(("clipping %f [%f-%f]", v, min_v, max_v));
				v = -1;
			} else if (v > 1) {
				LOG_DEBUG(("clipping %f [%f-%f]", v, min_v, max_v));
				v = 1;
			}
			*dst++ = (int)(v * 32767);
		}
		for(; i < WINDOW_SIZE; ++i) {
			overlap_data[channel_idx][i - WINDOW_SIZE / 2] = mdct.data[i];
		}
	}
}

void Source::_update_position(const int dp) {
	//LOG_DEBUG(("update_position(%d)", dp));
	position += dp;
	
	for(int i = 0; i < 2; ++i) {
		Buffer & buf = sample3d[i];
		buf.pop(dp * 2);
	}
	
	int src_n = (int)sample->_data.get_size() / sample->_spec.channels / 2;
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

float Source::_process(clunk::Buffer &buffer, unsigned dst_ch, const v3f &delta_position, float fx_volume, float pitch) {
	s16 * dst = static_cast<s16*>(buffer.get_ptr());
	unsigned dst_n = (unsigned)buffer.get_size() / dst_ch / 2;
	const s16 * src = static_cast<const s16 *>(sample->_data.get_ptr());
	if (src == NULL)
		throw_ex(("uninitialized sample used (%p)", (void *)sample));

	pitch *= this->pitch * sample->pitch;
	if (pitch <= 0)
		throw_ex(("pitch %g could not be negative or zero", pitch));
		
	unsigned src_ch = sample->_spec.channels; 
	unsigned src_n = (unsigned)sample->_data.get_size() / src_ch / 2;

	float vol = fx_volume * gain * sample->gain;
	
	if (vol > 1)
		vol = 1;

	if (vol < 0 || (int)floor(MaxMixVolume * vol + 0.5f) <= 0) {
		_update_position((int)(dst_n * pitch));
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
			
				s16 v = 0;
				if (loop || (p >= 0 && p < (int)src_n)) {
					p %= src_n;
					if (p < 0)
						p += src_n;
					
					if (c < src_ch) {
						v = src[p * src_ch + c];
					} else {
						v = src[p * src_ch];//expand mono channel if needed
					}

					if (panning != 0 && c < 2) {
						bool left = c == 0;
						int v0 = (int)((1.0f + panning * (left? -1: 1)) * v);
						if (v0 > 32767) {
							v = 32767;
						} else if (v0 < -32767) {
							v = -32767;
						} else {
							v = (s16)v0;
						}
					}
				}
				dst[i * dst_ch + c] = v;
			}
		}
		_update_position((int)(dst_n * pitch));
		return vol;
	}
	
	//LOG_DEBUG(("data: %p, angles: %d", (void *) kemar_data, angles));
	_update_position(0);
	
	if (position >= (int)src_n) {
		//LOG_ERROR(("process called on inactive source"));
		return 0;
	}

	float t_idt, angle_gr, left_to_right_amp;
	idt_iit(delta_position, t_idt, angle_gr, left_to_right_amp);

	const int kemar_sector_size = 360 / angles;
	const int kemar_idx_right = ((int)angle_gr + kemar_sector_size / 2) / kemar_sector_size;
	const int kemar_idx_left = ((360 - (int)angle_gr + kemar_sector_size / 2) / kemar_sector_size) % angles;
	//LOG_DEBUG(("%g (of %d)-> left: %d, right: %d", angle_gr, angles, kemar_idx_left, kemar_idx_right));
	
	int idt_offset = (int)(t_idt * sample->_spec.sample_rate);

	int window = 0;
	while(sample3d[0].get_size() < dst_n * 2 || sample3d[1].get_size() < dst_n * 2) {
		hrtf(window, 0, sample3d[0], src, src_ch, src_n, idt_offset, kemar_data, kemar_idx_left, left_to_right_amp > 1? 1: 1 / left_to_right_amp);
		hrtf(window, 1, sample3d[1], src, src_ch, src_n, idt_offset, kemar_data, kemar_idx_right, left_to_right_amp > 1? left_to_right_amp: 1);
		++window;
	}
	assert(sample3d[0].get_size() >= dst_n * 2 && sample3d[1].get_size() >= dst_n * 2);
	
	//LOG_DEBUG(("angle: %g", angle_gr));
	//LOG_DEBUG(("idt offset %d samples", idt_offset));
	s16 * src_3d[2] = { static_cast<s16 *>(sample3d[0].get_ptr()), static_cast<s16 *>(sample3d[1].get_ptr()) };
	
	//LOG_DEBUG(("size1: %u, %u, needed: %u\n%s", (unsigned)sample3d[0].get_size(), (unsigned)sample3d[1].get_size(), dst_n, sample3d[0].dump().c_str()));
	
	for(unsigned i = 0; i < dst_n; ++i) {
		for(unsigned c = 0; c < dst_ch; ++c) {
			dst[i * dst_ch + c] = src_3d[c][i];
		}
	}
	
	_update_position((int)(dst_n * pitch));
	//LOG_DEBUG(("size2: %u, %u, needed: %u", (unsigned)sample3d[0].get_size(), (unsigned)sample3d[1].get_size(), dst_n));
	return vol;
}

#include "kemar.h"

void Source::get_kemar_data(kemar_ptr & kemar_data, int & elev_n, const v3f &pos) {
	
	kemar_data = NULL;
	elev_n = 0;
	if (pos.is0())
		return;

#ifdef _WINDOWS
       float len = (float)_hypot(pos.x, pos.y);
#else
       float len = (float)hypot(pos.x, pos.y);
#endif

	int elev_gr = (int)(180 * atan2f(pos.z, len) / (float)M_PI);
	//LOG_DEBUG(("elev = %d (%g %g %g)", elev_gr, pos.x, pos.y, pos.z));

	for(size_t i = 0; i < KemarElevationCount; ++i)
	{
		const kemar_elevation_data &elev = ::kemar_data[i];
		if (elev_gr < elev.elevation + KemarElevationStep / 2)
		{
			//LOG_DEBUG(("used elevation %d", elev.elevation)); 
			kemar_data = elev.data;
			elev_n = elev.samples;
			break;
		}
	}
}

Source::~Source() {}

void Source::fade_out(const float sec) {
	fadeout = fadeout_total = (int)(sample->_spec.sample_rate * sec);
}
