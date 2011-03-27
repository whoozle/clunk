/* libClunk - cross-platform 3D audio API built on top SDL library
 * Copyright (C) 2007-2008 Netive Media Group
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * Contributed by Tai Chi Minh Ralph Eastwood <tcmreastwood@gmail.com>
*/

#include "context.h"
#include "source.h"
#include "stream.h"

using namespace clunk;
#include "clunk_c.h"

CLUNKCAPI clunk_context *clunk_context_create(int sample_rate, const Uint8 channels, int period_size)
{
	Context *ctx = new Context();
	ctx->init(sample_rate, channels, period_size);
	return ctx;
}

CLUNKCAPI void clunk_context_set_max_sources(clunk_context *ctx, int sources)
{
	ctx->set_max_sources(sources);
}

CLUNKCAPI void clunk_context_save(clunk_context *ctx, const char *file)
{
	ctx->save(file);
}

CLUNKCAPI void clunk_context_destroy(clunk_context *ctx)
{
	ctx->deinit();
	delete ctx;
}

CLUNKCAPI clunk_object *clunk_context_create_object(clunk_context *ctx)
{
	return ctx->create_object();
}

CLUNKCAPI clunk_sample *clunk_context_create_sample(clunk_context *ctx)
{
	return ctx->create_sample();
}

CLUNKCAPI void clunk_context_play(clunk_context *ctx, int id, clunk_stream *stream, int loop)
{
	ctx->play(id, (Stream *)stream->internal_stream, loop ? true : false);
}

CLUNKCAPI int clunk_context_playing(clunk_context *ctx, int id)
{
	return ctx->playing(id) ? 1 : 0;
}

CLUNKCAPI void clunk_context_pause(clunk_context *ctx, int id)
{
	ctx->pause(id);
}

CLUNKCAPI void clunk_context_stop(clunk_context *ctx, int id)
{
	ctx->stop(id);
}

CLUNKCAPI void clunk_context_set_volume(clunk_context *ctx, int id, float volume)
{
	ctx->set_volume(id, volume);
}

CLUNKCAPI void clunk_context_set_fx_volume(clunk_context *ctx, float volume)
{
	ctx->set_fx_volume(volume);
}

CLUNKCAPI void clunk_context_convert(clunk_context *ctx, clunk_buffer *dst, clunk_buffer *src, int rate, const Uint16 format, const Uint8 channels)
{
	ctx->convert(*dst, *src, rate, format, channels);
}

CLUNKCAPI clunk_object *clunk_context_get_listener(clunk_context *ctx)
{
	return ctx->get_listener();
}

CLUNKCAPI clunk_distance_model *clunk_context_get_distance_model(clunk_context *ctx)
{
	return &ctx->get_distance_model();
}

CLUNKCAPI void clunk_context_set_distance_model(clunk_context *ctx, clunk_distance_model *dm)
{
	ctx->set_distance_model(*dm);
}

CLUNKCAPI void clunk_context_stop_all(clunk_context *ctx)
{
	ctx->stop_all();
}

CLUNKCAPI void clunk_object_update(clunk_object *obj, clunk_v3 *pos, clunk_v3 *vel, clunk_v3 *dir)
{
	v3<float> p(pos->x, pos->y, pos->z), v(vel->x, vel->y, vel->z), d(dir->x, dir->y, dir->z);
	obj->update(p, v, d);
}

CLUNKCAPI void clunk_object_set_position(clunk_object *obj, clunk_v3 *pos)
{
	v3<float> p(pos->x, pos->y, pos->z);
	obj->set_position(p);
}

CLUNKCAPI void clunk_object_set_velocity(clunk_object *obj, clunk_v3 *vel)
{
	v3<float> v(vel->x, vel->y, vel->z);
	obj->set_velocity(v);
}

CLUNKCAPI void clunk_object_set_direction(clunk_object *obj, clunk_v3 *dir)
{
	v3<float> d(dir->x, dir->y, dir->z);
	obj->set_direction(d);
}

CLUNKCAPI void clunk_object_play(clunk_object *obj, const char *name, clunk_source *source)
{
	obj->play(name, source);
}

CLUNKCAPI void clunk_object_play_index(clunk_object *obj, int index, clunk_source *source)
{
	obj->play(index, source);
}

CLUNKCAPI int clunk_object_playing(clunk_object *obj, const char *name)
{
	return obj->playing(name) ? 1 : 0;
}

CLUNKCAPI int clunk_object_playing_index(clunk_object *obj, int index)
{
	return obj->playing(index) ? 1 : 0;
}

CLUNKCAPI void clunk_object_cancel(clunk_object *obj, const char *name, float fadeout)
{
	obj->cancel(name, fadeout);
}

CLUNKCAPI void clunk_object_cancel_index(clunk_object *obj, int index, float fadeout)
{
	obj->cancel(index, fadeout);
}

CLUNKCAPI void clunk_object_cancel_all(clunk_object *obj, int force, float fadeout)
{
	obj->cancel_all(force ? true : false, fadeout);
}

CLUNKCAPI void clunk_object_fade_out(clunk_object *obj, const char *name, float fadeout)
{
	obj->fade_out(name, fadeout);
}

CLUNKCAPI void clunk_object_fade_out_index(clunk_object *obj, int index, float fadeout)
{
	obj->fade_out(index, fadeout);
}

CLUNKCAPI int clunk_object_active(clunk_object *obj)
{
	return obj->active() ? 1 : 0;
}

CLUNKCAPI void clunk_object_auto_delete(clunk_object *obj)
{
	obj->autodelete();
}

CLUNKCAPI void clunk_object_set_loop(clunk_object *obj, const char *name, const int loop)
{
	obj->set_loop(name, loop ? true : false);
}

CLUNKCAPI void clunk_object_set_loop_index(clunk_object *obj, int index, const int loop)
{
	obj->set_loop(index, loop ? true : false);
}

CLUNKCAPI int clunk_object_get_loop(clunk_object *obj, const char *name)
{
	return obj->get_loop(name) ? 1 : 0;
}

CLUNKCAPI int clunk_object_get_loop_index(clunk_object *obj, int index)
{
	return obj->get_loop(index) ? 1 : 0;
}

CLUNKCAPI const char *clunk_sample_get_name(clunk_sample *smp)
{
	return smp->name.c_str();
}

CLUNKCAPI void clunk_sample_set_name(clunk_sample *smp, const char *name)
{
	smp->name = std::string(name);
}

CLUNKCAPI float clunk_sample_get_gain(clunk_sample *smp)
{
	return smp->gain;
}

CLUNKCAPI void clunk_sample_set_gain(clunk_sample *smp, float gain)
{
	smp->gain = gain;
}

CLUNKCAPI float clunk_sample_get_pitch(clunk_sample *smp)
{
	return smp->pitch;
}

CLUNKCAPI void clunk_sample_set_pitch(clunk_sample *smp, float pitch)
{
	smp->pitch = pitch;
}

CLUNKCAPI void clunk_sample_init(clunk_sample *smp, clunk_buffer *data, int rate, Uint16 format, Uint8 channels)
{
	smp->init(*data, rate, format, channels);
}

CLUNKCAPI void clunk_sample_load(clunk_sample *smp, const char *file)
{
	smp->load(file);
}

CLUNKCAPI void clunk_sample_generate_sine(clunk_sample *smp, int freq, float len)
{
	smp->generateSine(freq, len);
}

CLUNKCAPI float clunk_sample_length(clunk_sample *smp)
{
	return smp->length();
}

CLUNKCAPI clunk_source *clunk_source_create(const clunk_sample *sample, const int loop, const clunk_v3 *delta, float gain, float pitch, float panning)
{
	return new Source(sample, loop ? true : false, v3<float>(delta->x, delta->y, delta->z), gain, pitch, panning);
}

CLUNKCAPI void clunk_source_destroy(clunk_source *src)
{
	delete src;
}

CLUNKCAPI const clunk_sample *clunk_source_get_sample(clunk_source *src)
{
	return src->sample;
}

CLUNKCAPI int clunk_source_get_loop(clunk_source *src)
{
	return src->loop ? 1 : 0;
}

CLUNKCAPI void clunk_source_set_loop(clunk_source *src, int loop)
{
	src->loop = loop ? true : false;
}

CLUNKCAPI clunk_v3 clunk_source_get_delta(clunk_source *src)
{
	clunk_v3 v;
	v.x = src->delta_position.x;
	v.y = src->delta_position.y;
	v.z = src->delta_position.z;
	return v;
}

CLUNKCAPI void clunk_source_set_delta(clunk_source *src, clunk_v3 *delta)
{
	src->delta_position = v3<float>(delta->x, delta->y, delta->z);
}

CLUNKCAPI float clunk_source_get_gain(clunk_source *src)
{
	return src->gain;
}

CLUNKCAPI void clunk_source_set_gain(clunk_source *src, float gain)
{
	src->gain = gain;
}

CLUNKCAPI float clunk_source_get_pitch(clunk_source *src)
{
	return src->pitch;
}

CLUNKCAPI void clunk_source_set_pitch(clunk_source *src, float pitch)
{
	src->pitch = pitch;
}

CLUNKCAPI float clunk_source_get_panning(clunk_source *src)
{
	return src->panning;
}

CLUNKCAPI void clunk_source_set_panning(clunk_source *src, float panning)
{
	src->panning = panning;
}

CLUNKCAPI int clunk_source_playing(clunk_source *src)
{
	return src->playing() ? true : false;
}

CLUNKCAPI void clunk_source_fade_out(clunk_source *src, float sec)
{
	src->fade_out(sec);
}

class StreamWrapped : public Stream {
private:
	clunk_stream *cs;
public:
	StreamWrapped(clunk_stream *s)
	{
		cs = s;
	}
	void rewind()
	{
		cs->rewind(cs->user_data);
	}
	bool read(clunk::Buffer &data, unsigned hint)
	{
		return cs->read(cs->user_data, &data, hint) ? true : false;
	}
	~StreamWrapped()
	{
	}
};

CLUNKCAPI clunk_stream *clunk_stream_create(clunk_rewind rewind, clunk_read read, void *user_data)
{
	clunk_stream *cs = new clunk_stream();
	cs->internal_stream = (void *)new StreamWrapped(cs);
	cs->rewind = rewind;
	cs->read = read;
	cs->user_data = user_data;
	return cs;
}

CLUNKCAPI void clunk_stream_destroy(clunk_stream *cs)
{
	delete (StreamWrapped *)cs->internal_stream;
	delete cs;
}

CLUNKCAPI clunk_buffer *clunk_buffer_create(void)
{
	return new Buffer();
}

CLUNKCAPI clunk_buffer *clunk_buffer_create_copy(const clunk_buffer *buffer)
{
	return new Buffer(*buffer);
}

CLUNKCAPI clunk_buffer *clunk_buffer_create_size(size_t size)
{
	return new Buffer(size);
}

CLUNKCAPI void clunk_buffer_destroy(clunk_buffer *buffer)
{
	delete buffer;
}

CLUNKCAPI void *clunk_buffer_get_ptr(clunk_buffer *buffer)
{
	return buffer->get_ptr();
}

CLUNKCAPI const size_t clunk_buffer_get_size(clunk_buffer *buffer)
{
	return buffer->get_size();
}

CLUNKCAPI void clunk_buffer_set_size(clunk_buffer *buffer, size_t s)
{
	buffer->set_size(s);
}

CLUNKCAPI void clunk_buffer_set_data(clunk_buffer *buffer, const void *p, const size_t s)
{
	buffer->set_data(p, s);
}

CLUNKCAPI void clunk_buffer_set_data_own(clunk_buffer *buffer, void *p, const size_t s, int own)
{
	buffer->set_data(p, s, own ? true : false);
}

CLUNKCAPI void clunk_buffer_fill(clunk_buffer *buffer, int b)
{
	buffer->fill(b);
}

CLUNKCAPI void clunk_buffer_append(clunk_buffer *buffer, const clunk_buffer *other)
{
	buffer->append(*other);
}

CLUNKCAPI void clunk_buffer_append_data(clunk_buffer *buffer, const void *data, const size_t size)
{
	buffer->append(data, size);
}

CLUNKCAPI void *clunk_buffer_reserve(clunk_buffer *buffer, int more)
{
	return buffer->reserve(more);
}

CLUNKCAPI const char *clunk_buffer_dump(clunk_buffer *buffer)
{
	return buffer->dump().c_str();
}

CLUNKCAPI void clunk_buffer_pop(clunk_buffer *buffer, size_t n)
{
	return buffer->pop(n);
}

CLUNKCAPI void clunk_audio_lock(void)
{
	SDL_LockAudio();
}

CLUNKCAPI void clunk_audio_unlock(void)
{
	SDL_UnlockAudio();
}

CLUNKCAPI clunk_distance_model *clunk_distance_model_create(clunk_distance_model_type type, int clamped, float max_distance)
{
	return new DistanceModel(type, clamped ? true : false, max_distance);
}

CLUNKCAPI clunk_distance_model *clunk_distance_model_create_custom(clunk_distance_model_type type, int clamped, float max_distance, float reference_distance, float rolloff_factor, float doppler_factor, float speed_of_sound, float distance_divisor, unsigned same_sounds_limit)
{
	clunk_distance_model *dm = new DistanceModel(type, clamped ? true : false, max_distance);
	dm->reference_distance = reference_distance;
	dm->rolloff_factor = rolloff_factor;
	dm->doppler_factor = doppler_factor;
	dm->speed_of_sound = speed_of_sound;
	dm->distance_divisor = distance_divisor;
	dm->same_sounds_limit = same_sounds_limit;
	return dm;
}

CLUNKCAPI void clunk_distance_model_destroy(clunk_distance_model *dm)
{
	delete dm;
}
