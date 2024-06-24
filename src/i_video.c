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
// DESCRIPTION:
//	System specific interface stuff.
//
//-----------------------------------------------------------------------------

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"
#include "i_video.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

SDL_Window *sdl_win = NULL;
SDL_Surface *sdl_sur = NULL;
SDL_Surface *sdl_pal_sur = NULL;
SDL_Color screen_pal[256];

// Called by DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		I_Error("Couldn't initialize SDL2 video: %s", SDL_GetError());
		return;
	}
	
	sdl_win = SDL_CreateWindow("LinuxDOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_SHOWN);

	if(!sdl_win) {
		I_Error("Couldn't create a SDL window: %s", SDL_GetError());
		return;
	}
	
	sdl_sur = SDL_GetWindowSurface(sdl_win);

	if(!sdl_sur) {
		I_Error("Couldn't get the SDL surface: %s", SDL_GetError());
		return;
	}

	sdl_pal_sur = SDL_CreateRGBSurfaceFrom(screens[0], SCREENWIDTH, SCREENHEIGHT, 8, SCREENWIDTH, 0x00, 0x00, 0x00, 0x00);

	if(!sdl_pal_sur) {
		I_Error("Couldn't get the SDL paletted surface: %s", SDL_GetError());
		return;
	}

	SDL_UpdateWindowSurface(sdl_win);

	for(Uint16 i = 0; i < 256; i++)
		screen_pal[i].a = SDL_ALPHA_OPAQUE;
};

void I_ShutdownGraphics(void) {
	SDL_FreeSurface(sdl_pal_sur);
	SDL_DestroyWindowSurface(sdl_win);
	SDL_DestroyWindow(sdl_win);
};

// Takes full 8 bit values.
void I_SetPalette (byte* palette) {
	for(Uint16 i = 0, j = 0; i < 256; i++, j += 3) {
		screen_pal[i].r = palette[j];
		screen_pal[i].g = palette[j + 1];
		screen_pal[i].b = palette[j + 2];
	};

	SDL_SetPaletteColors(sdl_pal_sur->format->palette, screen_pal, 0, 256);
};

void I_UpdateNoBlit(void) {
	// what is this?
};

void I_FinishUpdate(void) {
	SDL_LockSurface(sdl_pal_sur);
	memcpy(sdl_pal_sur->pixels, screens[0], SCREENWIDTH * SCREENHEIGHT);
	SDL_UnlockSurface(sdl_pal_sur);

	SDL_BlitSurface(sdl_pal_sur, NULL, sdl_sur, NULL);
	SDL_UpdateWindowSurface(sdl_win);
};

void I_ReadScreen(byte* scr) {
	memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
};

void I_StartFrame(void) {
	// er?
};

void I_GetEvent(void) {
	// [TODO]
};

void I_StartTic(void) {
	if(!sdl_win)
		return;

	SDL_WarpMouseInWindow(sdl_win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
};
