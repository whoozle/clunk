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

#ifndef CLUNK_C_H___
#define CLUNK_C_H___
#include <stdlib.h>
#include "export_clunk.h"

#ifdef __cplusplus
#include "context.h"
#include "source.h"
#include "stream.h"
typedef clunk::Context clunk_context;
typedef clunk::Object clunk_object;
typedef clunk::Source clunk_source;
typedef clunk::Sample clunk_sample;
typedef clunk::Buffer clunk_buffer;
typedef clunk::DistanceModel clunk_distance_model;
typedef clunk::DistanceModel::Type clunk_distance_model_type;
#else
typedef void clunk_context;
typedef void clunk_object;
typedef void clunk_source;
typedef void clunk_sample;
typedef void clunk_buffer;
typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef enum {Inverse, Linear, Exponent} clunk_distance_model_type;
typedef void clunk_distance_model;
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*clunk_read)(void *user_data, clunk_buffer *buffer, unsigned hint);
typedef void (*clunk_rewind)(void *user_data);
typedef struct {
	float x, y, z;
} clunk_v3;
typedef struct {
	clunk_rewind rewind;
	clunk_read read;
	void *user_data;
	void *internal_stream;
} clunk_stream;
#ifdef __cplusplus
}
#endif
#endif
CLUNKCAPI clunk_context *clunk_context_create(int sample_rate, const Uint8 channels, int period_size);
CLUNKCAPI void clunk_context_set_max_sources(clunk_context *ctx, int sources);
CLUNKCAPI void clunk_context_save(clunk_context *ctx, const char *file);
CLUNKCAPI void clunk_context_destroy(clunk_context *ctx);
CLUNKCAPI clunk_object *clunk_context_create_object(clunk_context *ctx);
CLUNKCAPI clunk_sample *clunk_context_create_sample(clunk_context *ctx);
CLUNKCAPI void clunk_context_play(clunk_context *ctx, int id, clunk_stream *stream, int loop);
CLUNKCAPI int clunk_context_playing(clunk_context *ctx, int id);
CLUNKCAPI void clunk_context_pause(clunk_context *ctx, int id);
CLUNKCAPI void clunk_context_stop(clunk_context *ctx, int id);
CLUNKCAPI void clunk_context_set_volume(clunk_context *ctx, int id, float volume);
CLUNKCAPI void clunk_context_set_fx_volume(clunk_context *ctx, float volume);
CLUNKCAPI void clunk_context_convert(clunk_context *ctx, clunk_buffer *dst, clunk_buffer *src, int rate, const Uint16 format, const Uint8 channels);
CLUNKCAPI clunk_object *clunk_context_get_listener(clunk_context *ctx);
CLUNKCAPI clunk_distance_model *clunk_context_get_distance_model(clunk_context *ctx);
CLUNKCAPI void clunk_context_set_distance_model(clunk_context *ctx, clunk_distance_model *dm);
CLUNKCAPI void clunk_context_stop_all(clunk_context *ctx);
CLUNKCAPI void clunk_object_update(clunk_object *obj, clunk_v3 *pos, clunk_v3 *vel, clunk_v3 *dir);
CLUNKCAPI void clunk_object_set_position(clunk_object *obj, clunk_v3 *pos);
CLUNKCAPI void clunk_object_set_velocity(clunk_object *obj, clunk_v3 *vel);
CLUNKCAPI void clunk_object_set_direction(clunk_object *obj, clunk_v3 *dir);
CLUNKCAPI void clunk_object_play(clunk_object *obj, const char *name, clunk_source *source);
CLUNKCAPI void clunk_object_play_index(clunk_object *obj, int index, clunk_source *source);
CLUNKCAPI int clunk_object_playing(clunk_object *obj, const char *name);
CLUNKCAPI int clunk_object_playing_index(clunk_object *obj, int index);
CLUNKCAPI void clunk_object_cancel(clunk_object *obj, const char *name, float fadeout);
CLUNKCAPI void clunk_object_cancel_index(clunk_object *obj, int index, float fadeout);
CLUNKCAPI void clunk_object_cancel_all(clunk_object *obj, int force, float fadeout);
CLUNKCAPI void clunk_object_fade_out(clunk_object *obj, const char *name, float fadeout);
CLUNKCAPI void clunk_object_fade_out_index(clunk_object *obj, int index, float fadeout);
CLUNKCAPI int clunk_object_active(clunk_object *obj);
CLUNKCAPI void clunk_object_auto_delete(clunk_object *obj);
CLUNKCAPI void clunk_object_set_loop(clunk_object *obj, const char *name, const int loop);
CLUNKCAPI void clunk_object_set_loop_index(clunk_object *obj, int index, const int loop);
CLUNKCAPI int clunk_object_get_loop(clunk_object *obj, const char *name);
CLUNKCAPI int clunk_object_get_loop_index(clunk_object *obj, int index);
CLUNKCAPI const char *clunk_sample_get_name(clunk_sample *smp);
CLUNKCAPI void clunk_sample_set_name(clunk_sample *smp, const char *name);
CLUNKCAPI float clunk_sample_get_gain(clunk_sample *smp);
CLUNKCAPI void clunk_sample_set_gain(clunk_sample *smp, float gain);
CLUNKCAPI float clunk_sample_get_pitch(clunk_sample *smp);
CLUNKCAPI void clunk_sample_set_pitch(clunk_sample *smp, float pitch);
CLUNKCAPI void clunk_sample_init(clunk_sample *smp, clunk_buffer *data, int rate, Uint16 format, Uint8 channels);
CLUNKCAPI void clunk_sample_load(clunk_sample *smp, const char *file);
CLUNKCAPI void clunk_sample_generate_sine(clunk_sample *smp, int freq, float len);
CLUNKCAPI float clunk_sample_length(clunk_sample *smp);
CLUNKCAPI clunk_source *clunk_source_create(const clunk_sample *sample, const int loop, const clunk_v3 *delta, float gain, float pitch, float panning);
CLUNKCAPI void clunk_source_destroy(clunk_source *src);
CLUNKCAPI const clunk_sample *clunk_source_get_sample(clunk_source *src);
CLUNKCAPI int clunk_source_get_loop(clunk_source *src);
CLUNKCAPI void clunk_source_set_loop(clunk_source *src, int loop);
CLUNKCAPI clunk_v3 clunk_source_get_delta(clunk_source *src);
CLUNKCAPI void clunk_source_set_delta(clunk_source *src, clunk_v3 *delta);
CLUNKCAPI float clunk_source_get_gain(clunk_source *src);
CLUNKCAPI void clunk_source_set_gain(clunk_source *src, float gain);
CLUNKCAPI float clunk_source_get_pitch(clunk_source *src);
CLUNKCAPI void clunk_source_set_pitch(clunk_source *src, float pitch);
CLUNKCAPI float clunk_source_get_panning(clunk_source *src);
CLUNKCAPI void clunk_source_set_panning(clunk_source *src, float panning);
CLUNKCAPI int clunk_source_playing(clunk_source *src);
CLUNKCAPI void clunk_source_fade_out(clunk_source *src, float sec);
CLUNKCAPI clunk_stream *clunk_stream_create(clunk_rewind rewind, clunk_read read, void *user_data);
CLUNKCAPI void clunk_stream_destroy(clunk_stream *cs);
CLUNKCAPI clunk_buffer *clunk_buffer_create(void);
CLUNKCAPI clunk_buffer *clunk_buffer_create_copy(const clunk_buffer *buffer);
CLUNKCAPI clunk_buffer *clunk_buffer_create_size(size_t size);
CLUNKCAPI void clunk_buffer_destroy(clunk_buffer *buffer);
CLUNKCAPI void *clunk_buffer_get_ptr(clunk_buffer *buffer);
CLUNKCAPI const size_t clunk_buffer_get_size(clunk_buffer *buffer);
CLUNKCAPI void clunk_buffer_set_size(clunk_buffer *buffer, size_t s);
CLUNKCAPI void clunk_buffer_set_data(clunk_buffer *buffer, const void *p, const size_t s);
CLUNKCAPI void clunk_buffer_set_data_own(clunk_buffer *buffer, void *p, const size_t s, int own);
CLUNKCAPI void clunk_buffer_fill(clunk_buffer *buffer, int b);
CLUNKCAPI void clunk_buffer_append(clunk_buffer *buffer, const clunk_buffer *other);
CLUNKCAPI void clunk_buffer_append_data(clunk_buffer *buffer, const void *data, const size_t size);
CLUNKCAPI void *clunk_buffer_reserve(clunk_buffer *buffer, int more);
CLUNKCAPI const char *clunk_buffer_dump(clunk_buffer *buffer);
CLUNKCAPI void clunk_buffer_pop(clunk_buffer *buffer, size_t n);
CLUNKCAPI void clunk_audio_lock(void);
CLUNKCAPI void clunk_audio_unlock(void);
CLUNKCAPI clunk_distance_model *clunk_distance_model_create(clunk_distance_model_type type, int clamped, float max_distance);
CLUNKCAPI clunk_distance_model *clunk_distance_model_create_custom(clunk_distance_model_type type, int clamped, float max_distance, float reference_distance, float rolloff_factor, float doppler_factor, float speed_of_sound, float distance_divisor, unsigned same_sounds_limit);
CLUNKCAPI void clunk_distance_model_destroy(clunk_distance_model *dm);
