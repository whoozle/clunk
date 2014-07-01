#include <clunk/context.h>
#include <clunk/backend/sdl/backend.h>
#include <clunk/source.h>
#include <clunk/wav_file.h>
#include <stdlib.h>
#ifdef _WINDOWS
#	include <Windows.h>
#	define usleep(us) ::Sleep(((us) + 999) / 1000)
#else
#	include <unistd.h>
#endif

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
			fft.data[i] = std::complex<float>((float)((i / 4) & 1), 0);
		}
		fft.fft();
		for(int i = 0; i < fft_type::N; ++i) {
			printf("%f, %f = %f\n", fft.data[i].real(), fft.data[i].imag(), std::abs(fft.data[i]));
		}
		fft.ifft();

		for(int i = 0; i < fft_type::N; ++i) {
			fft.data[i] -= std::complex<float>((float)((i / 4) & 1), 0);
			printf("%f, %f\n", fft.data[i].real(), fft.data[i].imag());
		}
		
		return 0;
	}

	clunk::sdl::Backend backend(44100, 2, 1024);
	clunk::Context &context = backend.get_context();
	
	clunk::Object * o = context.create_object();
	clunk::Sample * s = clunk::WavFile::load(context, "scissors.wav");
	clunk::Sample * h = clunk::WavFile::load(context, "helicopter.wav");

	static const int d = 3, n = 72;
	clunk::DistanceModel dm(clunk::DistanceModel::Exponent, false);
	dm.rolloff_factor = 0.7f;
	context.set_distance_model(dm);
	
	context.save("test_out.raw");
	backend.start();
/*	o->play("l", new clunk::Source(s, false, clunk::v3f(-d, 0, 0)));
	sleep(1);
	//o->play("c", new clunk::Source(s, false, clunk::v3f(0, 0, 0)));
	//sleep(1);
	o->play("r", new clunk::Source(s, false, clunk::v3f(d, 0, 0)));
	sleep(1);

	o->play("u", new clunk::Source(s, false, clunk::v3f(0, d, 0)));
	sleep(1);

	o->play("b", new clunk::Source(s, false, clunk::v3f(0, 0, d)));
	sleep(1);
*/
	o->play("h", new clunk::Source(h, true));

	for(int i = 0; i <= n; ++i) {
		float a = float(2 * M_PI * i / n);
		clunk::v3f pos(cos(a) * d, sin(a) * d * 2, 1);
		o->set_position(pos);
		printf("%g %g %g\n", pos.x, pos.y, pos.z);
		usleep(100000);
	}

/*	for(int i = 0; i <= n; ++i) {
		float a = M_PI * i / n;
		o->play("s", new clunk::Source(s, false, clunk::v3f(cos(a), -0.25f, sin(a)) * d));
		usleep(500000);
	}
*/	backend.stop();
	return 0;
}
