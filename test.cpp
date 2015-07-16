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
#include <clunk/ref_mdct_context.h>

#define WINDOW_BITS 9

typedef clunk::mdct_context<WINDOW_BITS, clunk::vorbis_window_func, float> mdct_type;
typedef clunk::ref_mdct_context<WINDOW_BITS, clunk::vorbis_window_func, float> ref_mdct_type;
typedef clunk::fft_context<WINDOW_BITS - 2, float> fft_type;

template<typename mdct_type>
struct mdct_test {
	mdct_type	_mdct;
	float		_input[mdct_type::N];
	float		_overlap[mdct_type::N2];

	mdct_test(): _input(), _overlap() { }
	void dump(int n = mdct_type::N)
	{
		for(int i = 0; i < n; ++i)
			printf("%+1.1g ", _mdct.data[i]);
	}

	void feed()
	{
		//printf("feeding ");
		for(int i = 0; i < mdct_type::N2; ++i)
		{
			float v = (rand() % 11) - 5;
			_input[i] = _input[i + mdct_type::N2];
			_input[i + mdct_type::N2] = v;
			//printf("%+g ", v);
		}
		printf("\ninput: ");
		for(int i = 0; i < mdct_type::N; ++i)
		{
			_mdct.data[i] = _input[i];
		}
		dump();
		_mdct.apply_window();
		_mdct.mdct();
		//printf("\n\tfreq: "); dump();
		_mdct.imdct();
		_mdct.apply_window();

		for(int i = 0; i < mdct_type::N2; ++i)
			_mdct.data[i] += _overlap[i];

		for(int i = 0; i < mdct_type::N2; ++i)
			_overlap[i] = _mdct.data[mdct_type::N2 + i];

		printf("\nresult: "); dump(mdct_type::N2);
		printf("\n");
	}
};

int main(int argc, char *argv[]) {

	if (argc > 1 && argv[1][0] == 'b' && argv[1][1] == 'm') {
		mdct_type mdct;
		for(int i = 0; i < 200000; ++i)
		{
			mdct.mdct();
			clunk::Buffer::unoptimize(mdct.data, mdct.N);
		}
		return 0;
	}
	if (argc > 1 && argv[1][0] == 'b' && argv[1][1] == 'r') {
		ref_mdct_type mdct;
		for(int i = 0; i < 200000; ++i)
		{
			mdct.mdct();
			clunk::Buffer::unoptimize(mdct.data, mdct.N);
		}
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
	if (argc > 1 && argv[1][0] == 'f')
	{
		printf("reference: \n");
		mdct_test<clunk::ref_mdct_context<3, clunk::sin_window_func, float> > test;
		test.feed();
		test.feed();
		test.feed();
		test.feed();
		printf("fft based: \n");
		mdct_test<clunk::mdct_context<3, clunk::sin_window_func, float> > test2;
		test2.feed();
		test2.feed();
		test2.feed();
		test2.feed();
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
