// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	System interface for sound.
//
//-----------------------------------------------------------------------------

#include <SDL2/SDL.h>

extern "C" {
	#include "doomtype.h"
	#include "z_zone.h"

	#include "i_system.h"
	#include "i_sound.h"
	#include "m_argv.h"
	#include "m_misc.h"
	#include "w_wad.h"
	#include "i_system.h"

	#include "doomdef.h"
};

#include <cmath>
#include <vector>
#include <cstdint>

namespace RSDOOM {
	namespace Sound {
		const int MAX_SOUNDS = 256;
		const int SAMPLE_RATE = 44100;
		const int SAMPLES_BUFFER = SAMPLE_RATE / 100;
		const int CHANNELS = 2;

		static_assert((CHANNELS > 0) && (CHANNELS <= 2), "Only mono and stereo is supported.");

		bool is_sound_initialized = false;

		typedef std::vector<float> samples_t;

		struct sound_t {
			bool		reserved = false;
			int			id = -1;
			float		vol = 0.0f;
			float		sep = 0.0f;
			float		pitch = 0.0f;
			int			handle = -1;
			float		time = 0.0f;
			int			time_off = 0;
			samples_t	samples;
			bool		playing = false;
		};

		sound_t sounds[MAX_SOUNDS];

		SDL_AudioSpec i_sdl_as_curr;
		SDL_AudioSpec i_sdl_as_want;
		SDL_AudioDeviceID i_sdl_devid = 0;

		void i_sdl_audio_callback(void *userdata, Uint8 *stream, int len) {
			// [TODO] : Calculate pitch and samples, for real, instead of using tricks.

			SDL_LockAudioDevice(i_sdl_devid);

			float *f = (float *)stream;

			for(int i = 0; i < (SAMPLES_BUFFER * CHANNELS); i++)
				f[i] = 0.0f;

			float total_sounds[SAMPLES_BUFFER];
			for(int i = 0; i < SAMPLES_BUFFER; i++)
				total_sounds[i] = 0.0f;

			for(int sound_i = 0; sound_i < MAX_SOUNDS; sound_i++) {
				if(sounds[sound_i].reserved == false) continue;

				int off = sounds[sound_i].time_off;

				for(int i = 0, j = 0; i < SAMPLES_BUFFER; i++, j += CHANNELS) {
					if((off + i) >= sounds[sound_i].samples.size()) {
						sounds[sound_i].reserved = false;
						sounds[sound_i].playing = false;
						sounds[sound_i].time = 0.0f;
						sounds[sound_i].time_off = 0;
						sounds[sound_i].samples.clear();

						break;
					}

					total_sounds[i] += 1.0f;

					float sample = sounds[sound_i].samples[off + i];
					if(sample < (-1.0f)) sample = -1.0f;
					if(sample > (+1.0f)) sample = +1.0f;

					sample *= sounds[sound_i].vol;

					float right_vol = 1.0f - sounds[sound_i].sep;
					if(right_vol < 0.0f) right_vol = 0.0f;
					if(right_vol > 1.0f) right_vol = 1.0f;

					float left_vol = sounds[sound_i].sep + 1.0f;
					if(left_vol < 0.0f) left_vol = 0.0f;
					if(left_vol > 1.0f) left_vol = 1.0f;

					f[j + 0] += sample * left_vol;
					if(CHANNELS > 1) f[j + 1] += sample * right_vol;
				};
				
				sounds[sound_i].time_off += SAMPLES_BUFFER;
				sounds[sound_i].time += (float)SAMPLES_BUFFER / (float)(float)SAMPLE_RATE;
			};

			for(int i = 0, j = 0; i < SAMPLES_BUFFER; i++, j += CHANNELS) {
				float div = total_sounds[i];
				if(div < 1.0f) div = 1.0f;

				f[j + 0] /= div;
				f[j + 1] /= div;
			};

			SDL_UnlockAudioDevice(i_sdl_devid);
		};

		extern "C" void I_InitSound(void) {
			if(is_sound_initialized)
				return;

			if(SDL_Init(SDL_INIT_AUDIO) < 0) {
				I_Error((char *)"Couldn't initialize audio: %s", SDL_GetError());
				return;
			}

			is_sound_initialized = true;

			SDL_zero(i_sdl_as_curr);
			SDL_zero(i_sdl_as_want);

			i_sdl_as_want.freq = (float)SAMPLE_RATE;
			i_sdl_as_want.format = AUDIO_F32;
			i_sdl_as_want.channels = 2;
			i_sdl_as_want.samples = SAMPLES_BUFFER;
			i_sdl_as_want.callback = &i_sdl_audio_callback;

			i_sdl_devid = SDL_OpenAudioDevice(NULL, SDL_FALSE, &i_sdl_as_want, &i_sdl_as_curr, SDL_AUDIO_ALLOW_FORMAT_CHANGE);

			SDL_PauseAudioDevice(i_sdl_devid, SDL_FALSE);
		};

		extern "C" void I_ShutdownSound(void) {
			SDL_CloseAudioDevice(i_sdl_devid);
			SDL_CloseAudio();
		};

		extern "C" void I_SetChannels(void) {
			// [TODO]
		};

		extern "C" void I_SetMusicVolume(int volume) {
			// [TODO]
		};

		//
		// Retrieve the raw data lump index
		//  for a given SFX name.
		//
		extern "C" int I_GetSfxLumpNum(sfxinfo_t *sfx) {
			char namebuff[10];
			sprintf(namebuff, "ds%.7s", sfx->name);
			return W_GetNumForName(namebuff);
		};

		samples_t load_sfx(int sfx_id) {
			typedef uint8_t u8;
			typedef uint16_t u16;
			typedef unsigned int uint;

			u8 *data = (u8 *)W_CacheLumpNum(S_sfx[sfx_id].lumpnum, PU_STATIC);

			samples_t out;
			out.clear();

			if(
				(data[0] != 0x03) ||
				(data[1] != 0x00) ||
				(data[6] != 0x00) ||
				(data[7] != 0x00)
			) {
				I_Error((char *)"Invalid Sfx header.\n");
				return out;
			}

			u16 sampling_rate = (((u16)data[3] << 8) | (u16)data[2]);
			uint samples_len = (uint)(((u16)data[5] << 8) | (u16)data[4]);
			u8 *samples = data + 8;

			float ratio = (float)SAMPLE_RATE / (float)sampling_rate;

			uint out_len = (uint)round((float)samples_len * ratio);

			out.reserve(out_len);

			for(uint i = 0; i < out_len; i++) {
				// [TODO] : Add lerp between samples.
				uint index = (uint)round((float)i / ratio);
				u8 sample_u8 = samples[index];
				float sample_fl = ((float)sample_u8 - 128.0f) / 128.0f;

				out.insert(out.begin() + i, sample_fl);
			};

			Z_Free(data);

			return out;
		};

		float sfx_get_volume(int vol) {
			return (float)vol / 16.0f;
		};

		float sfx_get_sep(int sep) {
			return (128.0f - (float)sep) / 128.0f;
		};

		float sfx_get_pitch(int pitch) {
			return (float)pitch / 128.0f;
		};

		//
		// Starting a sound means adding it
		//  to the current list of active sounds
		//  in the internal channels.
		// As the SFX info struct contains
		//  e.g. a pointer to the raw data,
		//  it is ignored.
		//
		extern "C" int I_StartSound(int id, int vol, int sep, int pitch, int priority) {
			for(int i = 0; i < MAX_SOUNDS; i++) {
				if(sounds[i].reserved)
					continue;

				sounds[i].reserved = true;
				sounds[i].handle = i;
				sounds[i].id = id;
				sounds[i].vol = sfx_get_volume(vol);
				sounds[i].sep = sfx_get_sep(sep);
				sounds[i].pitch = sfx_get_pitch(pitch);
				sounds[i].time = 0.0f;
				sounds[i].time_off = 0;
				sounds[i].samples = load_sfx(id);
				sounds[i].playing = true;

				return i;
			};

			I_Error((char *)"[Rivest's DOOM] No available sound slots.\n");

			return -1;
		};

		extern "C" void I_StopSound(int handle) {
			if((handle < 0) | (handle >= MAX_SOUNDS))
				return;

			sounds[handle].reserved = false;
		};

		extern "C" int I_SoundIsPlaying(int handle) {
			if((handle < 0) | (handle >= MAX_SOUNDS))
				return false;

			if(sounds[handle].reserved)
				return false;
			
			return sounds[handle].playing;
		};

		extern "C" void I_UpdateSound(void) {
			// empty...
		};

		extern "C" void I_SubmitSound(void) {
			// empty too... (SDL2 handles this for us).
		};

		extern "C" void I_UpdateSoundParams(int handle, int vol, int sep, int pitch) {
			if((handle < 0) | (handle >= MAX_SOUNDS))
				return;

			if(sounds[handle].reserved == false)
				return;

			sounds[handle].vol = sfx_get_volume(vol);
			sounds[handle].sep = sfx_get_sep(sep);
			sounds[handle].pitch = sfx_get_pitch(pitch);
		};

		extern "C" void I_InitMusic(void) {
			// [TODO]
		};

		extern "C" void I_ShutdownMusic(void) {
			// [TODO]
		};

		extern "C" void I_PlaySong(int handle, int looping) {
			// [TODO]
		};

		extern "C" void I_PauseSong(int handle) {
			// [TODO]
		};

		extern "C" void I_ResumeSong(int handle) {
			// [TODO]
		};

		extern "C" void I_StopSong(int handle) {
			// [TODO]
		};

		extern "C" int I_RegisterSong(void *data) {
			// [TODO]
			return -1;
		};

		extern "C" void I_UnRegisterSong(int handle) {
			// [TODO]
		};
	};
};
