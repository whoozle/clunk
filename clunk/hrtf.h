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

#ifndef CLUNK_HRTF_H
#define	CLUNK_HRTF_H

/* clunk - cross-platform 3D audio API built on top SDL library
 * Copyright (C) 2007-2014 Netive Media Group
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


#include <clunk/buffer.h>
#include <clunk/export_clunk.h>
#include <clunk/mdct_context.h>
#include <clunk/types.h>
#include <clunk/v3.h>
#include <clunk/window_function.h>
#include <clunk/kemar.h>

namespace clunk {

class CLUNKAPI Hrtf {
public: 
	enum { WINDOW_BITS = 9 };

private: 
	typedef mdct_context<WINDOW_BITS, vorbis_window_func, float> mdct_type;
	mdct_type _mdct;

public:
	enum { WINDOW_SIZE = mdct_type::N };

	Hrtf();

	///fills dst_buf with dst_ch (must be 2 for now) of binaural data, returns number of samples used
	unsigned process(unsigned sample_rate, clunk::Buffer &dst_buf, unsigned dst_ch,
			const clunk::Buffer &src_buf, unsigned src_ch,
			const v3f &position, float fx_volume);

	void skip(unsigned samples);

private:
	static void idt_iit(const v3f &position, float &idt_offset, float &angle_gr, float &left_to_right_amp);
	typedef const float (*kemar_ptr)[2][KemarPoints][2];
	void get_kemar_data(kemar_ptr & kemar_data, int & samples, const v3f &delta_position);

	//generate hrtf response for channel idx (0 left), in result.
	void hrtf(const unsigned channel_idx, s16 *dst, const s16 *src, int src_ch, int src_n, int idt_offset, const kemar_ptr& kemar_data, int kemar_idx, float freq_decay);

private:
	clunk::Buffer sample3d[2];
	float overlap_data[2][WINDOW_SIZE / 2];
};

}

#endif
