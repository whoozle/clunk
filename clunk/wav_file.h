#ifndef CLUNK_WAV_FILE_H
#define CLUNK_WAV_FILE_H

#include <string>

namespace clunk {
	class Sample;
	struct WavFile {
		static Sample *load(const std::string &fname);
	};
}

#endif
