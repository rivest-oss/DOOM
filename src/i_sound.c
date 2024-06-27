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

#include <stdlib.h>
#include <stdarg.h>

#include <math.h>

#include "z_zone.h"

#include "i_system.h"
#include "i_sound.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"

#include "doomdef.h"

#include <SDL2/SDL.h>

void I_InitSound(void) {
	if(SDL_Init(SDL_INIT_AUDIO) < 0) {
		I_Error("Couldn't initialize audio video: %s", SDL_GetError());
		return;
	}

	printf("[Rivest's DOOM Sound] I_InitSound() called.\n");
};

void I_ShutdownSound(void) {
	// [TODO]
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
	// [TODO]
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
	// [TODO]
};

void I_StopSound(int handle) {
	// [TODO]
};

int I_SoundIsPlaying(int handle) {
	// [TODO]
};

void I_UpdateSound(void) {
	// [TODO]
};

void I_SubmitSound(void) {
	// [TODO]
};

void I_UpdateSoundParams(int handle, int vol, int sep, int pitch) {
	// [TODO]
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
