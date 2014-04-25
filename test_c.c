#include "clunk_c.h"
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	static const int d = 2, n = 6;
	int i;
	clunk_context *context = clunk_context_create(44100, 2, 1024);
	clunk_object *o = clunk_context_create_object(context);
	clunk_sample *s = clunk_context_create_sample(context);

	clunk_sample_load(s, "scissors.wav");
	
	clunk_context_save(context, "test_out.raw");

	for(i = 0; i <= n; ++i) {
		float a = M_PI * i / n;
		clunk_v3 v;
		clunk_source *src;
		v.x = -cos(a) * d;
		v.y = -sin(a) * d;
		v.z = 0;
		src = clunk_source_create(s, 0, &v, 1, 1, 0);
		clunk_object_play(o, "s", src);
		usleep(500000);
	}

	for(i = 0; i <= n; ++i) {
		float a = M_PI * i / n;
		clunk_v3 v;
		clunk_source *src;
		v.x = cos(a) * d;
		v.y = -0.25f * d;
		v.z = sin(a) * d;
		src = clunk_source_create(s, 0, &v, 1, 1, 0);
		clunk_object_play(o, "s", src);
		usleep(500000);
	}
	
	return 0;
}
