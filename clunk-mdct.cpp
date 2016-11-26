#include <clunk/mdct_context.h>
#include <clunk/wav_file.h>
#include <clunk/window_function.h>
#include <memory>
#include <stdio.h>
#include <stdlib.h>

namespace
{
	template<typename Format, int Bits>
	struct Impl
	{
		typedef clunk::mdct_context<Bits, clunk::kbd_window_func> mdct_type;
		typedef typename Format::Type SampleType;

		static void Forward(const clunk::Buffer & buffer)
		{
			const SampleType Max = std::numeric_limits<SampleType>::max();
			size_t data_size = buffer.get_size() / sizeof(SampleType);
			const SampleType *data = static_cast<const SampleType *>(buffer.get_ptr());

			float input[mdct_type::N] = {};
			float overlap[mdct_type::N2] = {};

			mdct_type	mdct;

			printf("{ \"data\": [\n");
			for(size_t offset = 0; offset < data_size; offset += mdct_type::N2)
			{
				for(int i = 0; i < mdct_type::N2; ++i)
				{
					float v = 1.0f * ((offset + i) < data_size? data[offset + i]: 0) / Max;
					input[i] = input[i + mdct_type::N2];
					input[i + mdct_type::N2] = v;
					//printf("%+g ", v);
				}
				for(int i = 0; i < mdct_type::N; ++i)
				{
					mdct.data[i] = input[i];
				}
				mdct.apply_window();
				mdct.mdct();
				//printf("\n\tfreq: "); dump();
				if (offset > 0)
					printf(",\n");
				printf("[");
				for(int i = 0; i < mdct_type::N; ++i)
				{
					if (i > 0)
						printf(", ");
					printf("%f", mdct.data[i]);
				}
				printf("]");
				mdct.imdct();
				mdct.apply_window();

				for(int i = 0; i < mdct_type::N2; ++i)
					mdct.data[i] += overlap[i];

				for(int i = 0; i < mdct_type::N2; ++i)
					overlap[i] = mdct.data[mdct_type::N2 + i];
			}
			printf("\n]}\n");
		}
	};
}

int main(int argc, char **argv)
{
	if (argc < 4)
	{
		printf("usage: forward <window-bits, e.g. 9> file.wav\n");
		return 0;
	}
	using namespace clunk;
	if (argv[1][0] == 'f')
	{
		int bits = atoi(argv[2]);
		std::auto_ptr<WavFile> wav(WavFile::load(argv[3]));
		if (wav->spec().format != AudioSpec::S16)
			throw std::runtime_error("only s16 samples supported");
		if (wav->spec().channels != 1)
			throw std::runtime_error("only mono samples supported");

		switch(bits)
		{
#define F(BITS) case BITS:		Impl<AudioFormat<AudioSpec::S16>, BITS>::Forward(wav->data()); break
			F(2);F(3);F(4);F(5);F(6);F(7);F(8);F(9);F(10);F(11);F(12);F(13);F(14);F(15);
		}
	}
	return 0;
}
