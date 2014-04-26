#ifndef CLUNK_WAV_FILE_H
#define CLUNK_WAV_FILE_H

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
		~WavFile();

		void read();
		u32 read_32le();
		void read(Buffer &buffer, size_t size);
		void read_format(const Buffer &data);
		bool ok() const
		{ return _spec.channels != 0 && !_data.empty(); }

	public:
		static Sample * load(Context &context, const std::string &fname);
		static void save(Sample &sample, const std::string &fname);
	};
}

#endif
