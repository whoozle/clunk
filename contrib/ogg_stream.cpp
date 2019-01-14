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

#include "ogg_stream.h"
#include <clunk/sample.h>
#include <assert.h>
#include <stdexcept>

//This example is the public domain

static size_t stream_read_func  (void *ptr, size_t size, size_t nmemb, void *datasource) {
	//LOG_DEBUG(("read(%p, %u, %u)", ptr, (unsigned)size, (unsigned)nmemb));
	assert(datasource != NULL);
	
	FILE *file = (FILE *)datasource;
	int r = fread(ptr, size, nmemb, file);
	if (r <= 0)
		return r;
	
	return r / size;
}

static int    stream_seek_func  (void *datasource, ogg_int64_t offset, int whence) {
	//LOG_DEBUG(("seek(%u, %d)", (unsigned)offset, whence));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return fseek(file, offset, whence);
}

static int    stream_close_func (void *datasource) {
	//LOG_DEBUG(("close()"));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return fclose(file);
}

static long   stream_tell_func  (void *datasource) {
	//LOG_DEBUG(("tell"));
	assert(datasource != NULL);
	FILE *file = (FILE *)datasource;
	return ftell(file);
}

OggStream::OggStream(const std::string &fname) {
	_file = fopen(fname.c_str(), "rb");
	if (_file == NULL) {
		perror("fopen");
		throw std::runtime_error("cannot open file");
	}

	ov_callbacks ov_cb;
	memset(&ov_cb, 0, sizeof(ov_cb));

	ov_cb.read_func = stream_read_func;
	ov_cb.seek_func = stream_seek_func;
	ov_cb.tell_func = stream_tell_func;
	ov_cb.close_func = stream_close_func;

	int r = ov_open_callbacks(_file, &_ogg_stream, NULL, 0, ov_cb);
	if (r < 0) {
		throw std::runtime_error("ov_open_callbacks failed");
	}
	
	_vorbis_info = ov_info(&_ogg_stream, -1);

	_spec.sample_rate = _vorbis_info->rate;
	//LOG_DEBUG(("open(%s) : %d", fname.c_str(), sample_rate));
	_spec.format = clunk::AudioSpec::S16;
	_spec.channels = _vorbis_info->channels;

	//_vorbis_comment = ov_comment(&_ogg_stream, -1);
	assert(_vorbis_info != NULL);
}

void OggStream::rewind() {
	int r = ov_raw_seek(&_ogg_stream, 0);
	if (r != 0)
		throw std::runtime_error("ov_raw_seek");
}

bool OggStream::read(clunk::Buffer &data, unsigned hint) {
	if (hint == 0) 
		hint = 44100;
	
	data.set_size(hint);

	int section = 0;
	int r = ov_read(&_ogg_stream, (char *)data.get_ptr(), hint, 0, 2, 1, & section);
	//LOG_DEBUG(("ov_read(%d) = %d (section: %d)", hint, r, section));
	
	if(r >= 0) {
		data.set_size(r);
		
		return r != 0;
	}

	return false; //:(
}

OggStream::~OggStream() {}
