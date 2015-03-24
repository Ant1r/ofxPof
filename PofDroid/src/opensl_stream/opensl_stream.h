/*
 * Copyright 2012 Peter Brinkmann (peter.brinkmann@gmail.com)
 *
 * Based on sample code by Victor Lazzarini, available at
 * http://audioprograming.wordpress.com/2012/03/03/android-audio-streaming-with-opensl-es-and-the-ndk/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OPENSL_STREAM_H__
#define __OPENSL_STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Processing callback; takes a processing context (which is just a pointer to
 * whatever data you want to pass to the callback), the sample rate, the
 * buffer size in frames, the number of input and output channels, as well as
 * input and output buffers whose size will be the number of channels times
 * the number of frames per buffer.
 *
 * This function will be invoked on a dedicated audio thread, and so any data in
 * the context that may be modified concurrently must be protected (e.g., by gcc
 * atomics) to prevent race conditions.
 */
typedef void (*opensl_process_t)
    (void *context, int sample_rate, int buffer_frames,
     int input_channels, const short *input_buffer,
     int output_channels, short *output_buffer);

/*
 * Abstract data type for streaming audio with OpenSL.
 */
typedef struct _opensl_stream OPENSL_STREAM;

/*
 * Opens the audio device for the given sample rate and channel configuration;
 * registers an audio processing callback that will receive a context pointer
 * (which may be NULL if no context is needed).  The context is owned by the
 * caller.
 *
 * The buffer size is the buffer size (in frames) at which OpenSL will operate.
 * Ideally, it should be the native buffer size of OpenSL.  As of Android 4.2,
 * the AudioManager class in Java offers a method that will yield the
 * recommended sample rate and buffer size for OpenSL.
 *
 * If you don't know the native buffer size, or if your processing callback
 * cannot operate at the native buffer size, then you should use a smallish
 * buffer size, e.g., 64 frames. Somewhat surprisingly, large buffers will not
 * improve stability.
 * 
 * For the time being, the number of input and output channels must be 0, 1,
 * or 2; at least one of the channel numbers must be positive.
 *
 * Returns NULL on failure.
 */
OPENSL_STREAM *opensl_open(
    int sample_rate, int input_channels, int output_channels,
    int callback_buffer_size, opensl_process_t proc, void *context);

/*
 * Stops playback and frees all resources associated with the given stream,
 * except for the context pointer, which is owned by the caller; the cleanup of
 * the context (if any) is the responsibility of the caller.
 */
void opensl_close(OPENSL_STREAM *p);

/*
 * Returns nonzero value if the given stream is currently running.
 */
int opensl_is_running(OPENSL_STREAM *p);

/*
 * Starts the audio stream.
 *
 * Returns 0 on success.
 */
int opensl_start(OPENSL_STREAM *p);

/*
 * Pauses the audio stream.
 */
void opensl_pause(OPENSL_STREAM *p);

#ifdef __cplusplus
};
#endif

#endif // #ifndef __OPENSL_STREAM_H__
