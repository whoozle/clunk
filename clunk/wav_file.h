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

#ifndef CLUNK_WAV_FILE_H
#define CLUNK_WAV_FILE_H

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

#include <string>
#include <stdio.h>
#include <clunk/types.h>
#include <clunk/buffer.h>
#include <clunk/audio_spec.h>

namespace clunk {
	class Context;
	class Sample;

	class CLUNKAPI WavFile {
		FILE *		_f;
		AudioSpec	_spec;
		Buffer		_data;

	private:
		WavFile(FILE *f);

		void read();
		u32 read_32le();
		void read(Buffer &buffer, size_t size);
		void read_format(const Buffer &data);
		bool ok() const
		{ return _spec.channels != 0 && !_data.empty(); }

	public:
		WavFile(const AudioSpec & spec, const Buffer & data): _f(), _spec(spec), _data(data) {}
		~WavFile();
		const Buffer & data() const		{ return _data; }
		const AudioSpec & spec() const  { return _spec; }

		static WavFile * load(const std::string &fname);
		static Sample * load(Context &context, const std::string &fname);

		void save(const std::string &fname);
	};
}

#endif
