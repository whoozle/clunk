#ifndef CLUNK_SOURCE_H__
#define CLUNK_SOURCE_H__

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

#include <SDL_audio.h>
#include "export_clunk.h"
#include "v3.h"

struct kiss_fftr_state;

namespace clunk {

#ifndef CLUNK_WINDOW_SIZE
#	define CLUNK_WINDOW_SIZE 512
#endif

#ifndef CLUNK_WINDOW_OVERLAP
#	define CLUNK_WINDOW_OVERLAP 128
#endif

class Sample;
class Buffer;

//!class holding information about source. 
class CLUNKAPI Source {
public:
	///pointer to the sample holding audio data
	const Sample * const sample;
	
	///loop flag
	bool loop;
	///delta position from the object's center
	v3<float> delta_position; //0 - from the center of the object. 
	///gain
	float gain;
	///pitch, 2.0f - pitching up one octave
	float pitch;
	/*! 
		\brief constructs new source
		\param[in] sample audio data
		\param[in] loop loops sample
		\param[in] delta delta position. (0, 0) is the object's center.
		\param[in] gain gain
		\param[in] pitch pitch
	*/
	Source(const Sample * sample, const bool loop = false, const v3<float> &delta = v3<float>(), float gain = 1, float pitch = 1);
	/*! 
		\brief for the internal use only. DO NOT USE IT. 
		\internal for the internal use only. 
	*/
	float process(clunk::Buffer &buffer, unsigned ch, const v3<float> &position, const v3<float> &direction, float fx_volume, float pitch);
	///returns current source's status.
	bool playing() const;

	/*! 
		\brief for the internal use only. DO NOT USE IT. 
		\internal for the internal use only. 
	*/
	void update_position(const int dp);

	///fades out source. usually you do not need this method
	void fade_out(const float sec);
	
	~Source();
	
private: 
	typedef const float (*kemar_ptr)[2][512];
	void get_kemar_data(kemar_ptr & kemar_data, int & samples, const v3<float> &delta_position);

	static void idt(const v3<float> &delta, const v3<float> &direction, float &idt_offset, float &angle_gr);
	void hrtf(const unsigned channel_idx, clunk::Buffer &result, int dst_n, const Sint16 *src, int src_ch, int src_n, int idt_offset, const kemar_ptr& kemar_data, int kemar_idx);

	int position, fadeout, fadeout_total;
	
	bool use_overlap[2];
	Sint16 overlap_data[2][CLUNK_WINDOW_OVERLAP];
	
	kiss_fftr_state *fft_state, *ffti_state;
};
}

#endif
