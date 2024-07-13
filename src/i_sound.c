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

#include <SDL2/SDL_audio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include "doomtype.h"
#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "i_system.h"

#include "doomdef.h"

#include <SDL2/SDL.h>

#define RSD_MAX_SOUNDS 256

typedef struct {
	boolean	has;

	int		id;
	int		vol;
	int		sep;
	int		pitch;
	int		handle;
	boolean	playing;
} rsd_snd_s;

rsd_snd_s *rsd_snds_arr = NULL;

void I_InitSound(void) {
	rsd_snds_arr = (rsd_snd_s *)malloc(RSD_MAX_SOUNDS * sizeof(rsd_snd_s));
	if(rsd_snds_arr == NULL) {
		I_Error("Couldn't initialize the sounds array");
		return;
	}

	if(SDL_Init(SDL_INIT_AUDIO) < 0) {
		I_Error("Couldn't initialize audio video: %s", SDL_GetError());
		free(rsd_snds_arr);
		return;
	}

	printf("[Rivest's DOOM Sound] I_InitSound() called.\n");
};

void I_ShutdownSound(void) {
	if(rsd_snds_arr != NULL)
		free(rsd_snds_arr);

	SDL_CloseAudio();
};

void I_SetChannels(void) {
	// [TODO]
};

void I_SetMusicVolume(int volume) {
	// [TODO]
};

//
// Retrieve the raw data lump index
//  for a given SFX name.
//
int I_GetSfxLumpNum(sfxinfo_t *sfx) {
	char namebuff[9];
	sprintf(namebuff, "ds%.7s", sfx->name);
	return W_GetNumForName(namebuff);
};

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
//
int I_StartSound(int id, int vol, int sep, int pitch, int priority) {
	for(int i = 0; i < RSD_MAX_SOUNDS; i++) {
		if(rsd_snds_arr[i].has)
			continue;

		rsd_snds_arr[i].has = true;
		rsd_snds_arr[i].handle = (int)i;
		rsd_snds_arr[i].id = id;
		rsd_snds_arr[i].vol = vol;
		rsd_snds_arr[i].sep = sep;
		rsd_snds_arr[i].pitch = pitch;
		rsd_snds_arr[i].playing = true;

		return i;
	};

	return -1;
};

void I_StopSound(int handle) {
	if((handle < 0) | (handle >= RSD_MAX_SOUNDS))
		return;

	rsd_snds_arr[handle].has = false;
};

int I_SoundIsPlaying(int handle) {
	if((handle < 0) | (handle >= RSD_MAX_SOUNDS))
		return false;
	if(rsd_snds_arr[handle].has == false)
		return false;
	
	return rsd_snds_arr[handle].playing;
};

void I_UpdateSound(void) {
	// [TODO]
};

void I_SubmitSound(void) {
	// [TODO]
};

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch) {
	if((handle < 0) | (handle >= RSD_MAX_SOUNDS))
		return;
	if(rsd_snds_arr[handle].has == false)
		return;

	rsd_snds_arr[handle].vol = vol;
	rsd_snds_arr[handle].sep = sep;
	rsd_snds_arr[handle].pitch = pitch;
};

void I_InitMusic(void) {
	// [TODO]
};

void I_ShutdownMusic(void) {
	// [TODO]
};

void I_PlaySong(int handle, int looping) {
	// [TODO]
};

void I_PauseSong(int handle) {
	// [TODO]
};

void I_ResumeSong(int handle) {
	// [TODO]
};

void I_StopSong(int handle) {
	// [TODO]
};

int I_RegisterSong(void *data) {
	// [TODO]
};

void I_UnRegisterSong(int handle) {
	// [TODO]
};
