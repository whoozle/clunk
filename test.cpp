#include <clunk/context.h>
#include <clunk/backend/sdl/backend.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>
#include <stdlib.h>
#include <unistd.h>

#define WINDOW_BITS 9

typedef clunk::mdct_context<WINDOW_BITS, clunk::vorbis_window_func, float> mdct_type;
typedef clunk::fft_context<WINDOW_BITS - 2, float> fft_type;

int main(int argc, char *argv[]) {
	if (argc > 1 && argv[1][0] == 'b' && argv[1][1] == 'm') {
		mdct_type mdct;
		for(int i = 0; i < 1000000; ++i) 
			mdct.mdct();
		return 0;
	}
	if (argc > 1 && argv[1][0] == 'b' && argv[1][1] == 'f') {
		fft_type fft;
		for(int i = 0; i < 2000000; ++i) 
			fft.fft();
		return 0;
	}
	if (argc > 1 && argv[1][0] == 't') {
		fft_type fft;
		for(int i = 0; i < fft_type::N; ++i) {
			fft.data[i] = std::complex<float>((i / 4) & 1, 0);
		}
		fft.fft();
		for(int i = 0; i < fft_type::N; ++i) {
			printf("%f, %f = %f\n", fft.data[i].real(), fft.data[i].imag(), std::abs(fft.data[i]));
		}
		fft.ifft();

		for(int i = 0; i < fft_type::N; ++i) {
			fft.data[i] -= std::complex<float>((i / 4) & 1, 0);
			printf("%f, %f\n", fft.data[i].real(), fft.data[i].imag());
		}
		
		return 0;
	}

	clunk::sdl::Backend backend(44100, 2, 1024);
	clunk::Context &context = backend.get_context();
	
	clunk::Object * o = context.create_object();
	clunk::Sample * s = clunk::WavFile::load(context, "scissors.wav");

	static const int d = 2, n = 6;
	
	context.save("test_out.raw");
	backend.start();
/*	o->play("l", new clunk::Source(s, false, clunk::v3<float>(-d, 0, 0)));
	sleep(1);
	//o->play("c", new clunk::Source(s, false, clunk::v3<float>(0, 0, 0)));
	//sleep(1);
	o->play("r", new clunk::Source(s, false, clunk::v3<float>(d, 0, 0)));
	sleep(1);

	o->play("u", new clunk::Source(s, false, clunk::v3<float>(0, d, 0)));
	sleep(1);

	o->play("b", new clunk::Source(s, false, clunk::v3<float>(0, 0, d)));
	sleep(1);
*/

	for(int i = 0; i <= n; ++i) {
		float a = M_PI * i / n;
		o->play("s", new clunk::Source(s, false, clunk::v3<float>(-cos(a), -sin(a), 0) * d));
		usleep(500000);
	}

/*	for(int i = 0; i <= n; ++i) {
		float a = M_PI * i / n;
		o->play("s", new clunk::Source(s, false, clunk::v3<float>(cos(a), -0.25f, sin(a)) * d));
		usleep(500000);
	}
*/	usleep(500000);
	return 0;
}
