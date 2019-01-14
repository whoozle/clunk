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

#include <clunk/wav_file.h>
#include <clunk/sample.h>
#include <clunk/context.h>
#include <clunk/logger.h>
#include <stdio.h>
#include <stdexcept>
#include <memory>

namespace clunk {
	WavFile::WavFile(FILE *f) : _f(f) {}
	WavFile::~WavFile() { if (_f) fclose(_f); }

	u32 WavFile::read_32le()
	{
		u8 data[4];
		if (fread(data, 1, 4, _f) != 4)
			throw std::runtime_error("fread failed");
		return (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
	}

	void WavFile::read_format(const Buffer &data) {
		const u8 *src = static_cast<u8 *>(data.get_ptr());
		if (data.get_size() < 16)
			throw std::runtime_error("invalid header size");

		u16 format = src[0] | (src[1] << 8);
		if (format != 1)
			throw std::runtime_error("only PCM format supported");
		_spec.channels		= src[2] | (src[3] << 8);
		_spec.sample_rate	= src[4] | (src[5] << 8) | (src[6] << 16) | (src[7] << 24);
		format				= src[14] | (src[15] << 8);
		if (format != 8 && format != 16)
			throw std::runtime_error("invalid bits per sample size");
		LOG_DEBUG(("channels: %u, sample rate: %u, format: %u", _spec.channels, _spec.sample_rate, _spec.format));
	}

	void WavFile::read(Buffer &buffer, size_t size)
	{
		buffer.resize(size);
		if (fread(buffer.get_ptr(), 1, size, _f) != size)
			throw std::runtime_error("read failed");
	}

	void WavFile::read() {
		fseek(_f, 0, SEEK_SET);
		u32 riff = read_32le();
		read_32le();
		if (riff != 0x46464952)
			throw std::runtime_error("invalid riff file signature");
		u32 format = read_32le();
		if (format != 0x45564157)
			throw std::runtime_error("only wave format supported");

		while(!ok())
		{
			u32 id = read_32le();
			u32 size = read_32le();
			LOG_DEBUG(("id: 0x%08x, size: %u", id, size));
			if (id == 0x20746d66)
			{
				Buffer fmt;
				read(fmt, size);
				read_format(fmt);
			} else if (id == 0x61746164) {
				read(_data, size);

#if !defined(_WINDOWS) && __BYTE_ORDER == __BIG_ENDIAN
				if (_spec.bytes_per_sample() == 2)
				{
					u8 *ptr = static_cast<u8 *>(_data.get_ptr());
					for(size_t i = 0; i + 1 < _data.get_size(); i += 2, ptr += 2)
						std::swap(ptr[0], ptr[1]);
				}
#endif
			} else
				fseek(_f, size, SEEK_CUR);
		}
	}

	WavFile * WavFile::load(const std::string &fname)
	{
		FILE *f = fopen(fname.c_str(), "rb");
		if (!f)
			throw std::runtime_error("cannot open file: " + fname);

		std::unique_ptr<WavFile> wav(new WavFile(f));
		wav->read();
		return wav.release();
	}

	Sample * WavFile::load(Context &context, const std::string &fname) {
		std::unique_ptr<WavFile> wav(load(fname));
		std::unique_ptr<Sample> sample(context.create_sample());
		sample->init(wav->_data, wav->_spec);
		sample->name = fname;
		return sample.release();
	}

	namespace {
		void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file)
		{
			unsigned buf;
			while(num_bytes>0)
			{   buf = word & 0xff;
				fwrite(&buf, 1,1, wav_file);
				num_bytes--;
			word >>= 8;
			}
		}
	}

	void WavFile::save(const std::string &fname)
	{
		FILE *wav_file = fopen(fname.c_str(), "wb");
		if (!wav_file)
			throw std::runtime_error("cannot open " + fname + " for writing");

		unsigned byte_rate = _spec.sample_rate * _spec.channels * _spec.bytes_per_sample();
		unsigned num_samples = _data.get_size() / _spec.channels / _spec.bytes_per_sample();

		/* write RIFF header */
		fwrite("RIFF", 1, 4, wav_file);
		write_little_endian(36 + _spec.bytes_per_sample() * num_samples* _spec.channels, 4, wav_file);
		fwrite("WAVE", 1, 4, wav_file);

		/* write fmt  subchunk */
		fwrite("fmt ", 1, 4, wav_file);
		write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
		write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
		write_little_endian(_spec.channels, 2, wav_file);
		write_little_endian(_spec.sample_rate, 4, wav_file);
		write_little_endian(byte_rate, 4, wav_file);
		write_little_endian(_spec.channels * _spec.bytes_per_sample(), 2, wav_file);  /* block align */
		write_little_endian(8 * _spec.bytes_per_sample(), 2, wav_file);  /* bits/sample */

		/* write data subchunk */
		fwrite("data", 1, 4, wav_file);
		write_little_endian(_spec.bytes_per_sample() * num_samples * _spec.channels, 4, wav_file);
		fwrite(_data.get_ptr(), _data.get_size(), 1, wav_file);

		fclose(wav_file);
	}
}
