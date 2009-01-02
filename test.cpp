#include "context.h"
#include "source.h"
#include <stdlib.h>

int main() {
	clunk::Context context;
	context.init(44100, 2, 1024);
	
	clunk::Object * o = context.create_object();
	clunk::Sample * s = context.create_sample();

	s->load("scissors.wav");
	static const int d = 2, n = 6;
	
	context.save("test_out.raw");
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
