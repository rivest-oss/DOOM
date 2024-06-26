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
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

#include <stdio.h>

const unsigned int WIN_SHIFT_FACTOR = 2;

const unsigned int WIN_WIDTH = (SCREENWIDTH << WIN_SHIFT_FACTOR);
const unsigned int WIN_HEIGHT = (SCREENHEIGHT << WIN_SHIFT_FACTOR);

SDL_Window *sdl_win = NULL;
SDL_Surface *sdl_sur = NULL;
SDL_Surface *sdl_pal_sur = NULL;
SDL_Color sdl_pal_colors[256];

// Called by DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		I_Error("Couldn't initialize SDL2 video: %s", SDL_GetError());
		return;
	}
	
	sdl_win = SDL_CreateWindow("LinuxDOOM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);

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
		sdl_pal_colors[i].a = SDL_ALPHA_OPAQUE;
};

void I_ShutdownGraphics(void) {
	SDL_FreeSurface(sdl_pal_sur);
	SDL_DestroyWindowSurface(sdl_win);
	SDL_DestroyWindow(sdl_win);
};

// Takes full 8 bit values.
void I_SetPalette (byte* palette) {
	for(Uint16 i = 0, j = 0; i < 256; i++, j += 3) {
		sdl_pal_colors[i].r = palette[j];
		sdl_pal_colors[i].g = palette[j + 1];
		sdl_pal_colors[i].b = palette[j + 2];
	};

	SDL_SetPaletteColors(sdl_pal_sur->format->palette, sdl_pal_colors, 0, 256);
};

void I_UpdateNoBlit(void) {
	// what is this?
};

void I_FinishUpdate(void) {
	SDL_LockSurface(sdl_sur);
	SDL_LockSurface(sdl_pal_sur);

	memcpy(sdl_pal_sur->pixels, screens[0], SCREENWIDTH * SCREENHEIGHT);

	Uint8 *src_pix = (Uint8 *)sdl_pal_sur->pixels;
	Uint32 *dst_pix = (Uint32 *)sdl_sur->pixels;
	SDL_Color sdl_color;

	for(Uint32 y = 0, yw = 0; y < WIN_HEIGHT; y++, yw += WIN_WIDTH) {
		Uint32 src_y = ((y >> WIN_SHIFT_FACTOR) * SCREENWIDTH);

		for(Uint32 x = 0; x < WIN_WIDTH; x++) {
			Uint32 src_x = (x >> WIN_SHIFT_FACTOR);

			sdl_color = sdl_pal_colors[screens[0][src_y + src_x]];
			dst_pix[yw + x] =	((0xff << 24) |
								(sdl_color.r << 16) |
								(sdl_color.g << 8) |
								sdl_color.b);
		};
	};

	SDL_UnlockSurface(sdl_sur);
	SDL_UnlockSurface(sdl_pal_sur);

	SDL_UpdateWindowSurface(sdl_win);
};

void I_ReadScreen(byte* scr) {
	memcpy(scr, screens[0], SCREENWIDTH * SCREENHEIGHT);
};

void I_StartFrame(void) {
	// er?
};

int get_sdl_keycode(SDL_Event *sdl_ev) {
	int code = -1;

	switch(sdl_ev->key.keysym.scancode) {
		case SDL_SCANCODE_UP: code = KEY_UPARROW; break;
		case SDL_SCANCODE_DOWN: code = KEY_DOWNARROW; break;
		case SDL_SCANCODE_LEFT: code = KEY_LEFTARROW; break;
		case SDL_SCANCODE_RIGHT: code = KEY_RIGHTARROW; break;
		case SDL_SCANCODE_ESCAPE: code = KEY_ESCAPE; break;
		case SDL_SCANCODE_RETURN: code = KEY_ENTER; break;
		case SDL_SCANCODE_TAB: code = KEY_TAB; break;
		case SDL_SCANCODE_F1: code = KEY_F1; break;
		case SDL_SCANCODE_F2: code = KEY_F2; break;
		case SDL_SCANCODE_F3: code = KEY_F3; break;
		case SDL_SCANCODE_F4: code = KEY_F4; break;
		case SDL_SCANCODE_F5: code = KEY_F5; break;
		case SDL_SCANCODE_F6: code = KEY_F6; break;
		case SDL_SCANCODE_F7: code = KEY_F7; break;
		case SDL_SCANCODE_F8: code = KEY_F8; break;
		case SDL_SCANCODE_F9: code = KEY_F9; break;
		case SDL_SCANCODE_F10: code = KEY_F10; break;
		case SDL_SCANCODE_F11: code = KEY_F11; break;
		case SDL_SCANCODE_F12: code = KEY_F12; break;
		case SDL_SCANCODE_DELETE:
		case SDL_SCANCODE_BACKSPACE:
			code = KEY_BACKSPACE; break;
		case SDL_SCANCODE_PAUSE: code = KEY_PAUSE; break;
		case SDL_SCANCODE_KP_ENTER:
		case SDL_SCANCODE_EQUALS:
			code = KEY_EQUALS; break;
		case SDL_SCANCODE_KP_MINUS:
		case SDL_SCANCODE_MINUS:
			code = KEY_MINUS; break;
		case SDL_SCANCODE_LSHIFT:
		case SDL_SCANCODE_RSHIFT:
			code = KEY_RSHIFT; break;
		case SDL_SCANCODE_LCTRL:
		case SDL_SCANCODE_RCTRL:
			code = KEY_RCTRL; break;
		case SDL_SCANCODE_LALT:
		case SDL_SCANCODE_RALT:
			code = KEY_RALT; break;

		default: {
			if((sdl_ev->key.keysym.scancode >= 4) & (sdl_ev->key.keysym.scancode <= 29)) {
				code = sdl_ev->key.keysym.scancode + 0x5d;
			} else if(sdl_ev->key.keysym.scancode == 44) {
				code = 0x20;
			}
			
			break;
		};
	};

	return code;
};

int sdl_mouse_x = 0, sdl_mouse_y = 0;

void I_GetEvents(void) {
	event_t doom_ev;
	SDL_Event sdl_ev;

	while(SDL_PollEvent(&sdl_ev) != 0) {
		switch(sdl_ev.type) {
			case SDL_KEYDOWN: {
				doom_ev.type = ev_keydown;
				doom_ev.data1 = get_sdl_keycode(&sdl_ev);
				D_PostEvent(&doom_ev);
				break;
			};

			case SDL_KEYUP: {
				doom_ev.type = ev_keyup;
				doom_ev.data1 = get_sdl_keycode(&sdl_ev);
				D_PostEvent(&doom_ev);
				break;
			};

			case SDL_MOUSEBUTTONDOWN: {
				doom_ev.type = ev_mouse;
				doom_ev.data1 = (sdl_ev.button.button == SDL_BUTTON_LEFT);
				// [TODO] Improve this ^
				doom_ev.data2 = doom_ev.data3 = 0;
				D_PostEvent(&doom_ev);
				break;
			};

			case SDL_MOUSEBUTTONUP: {
				doom_ev.type = ev_mouse;
				doom_ev.data1 = (sdl_ev.button.button != SDL_BUTTON_LEFT);
				// [TODO] Improve this ^
				doom_ev.data2 = doom_ev.data3 = 0;
				D_PostEvent(&doom_ev);
				break;
			};

			case SDL_MOUSEMOTION: {
				Uint32 m_state = SDL_GetRelativeMouseState(&sdl_mouse_x, &sdl_mouse_y);
			
				doom_ev.type = ev_mouse;
				doom_ev.data1 = 0;
				doom_ev.data2 = ((sdl_mouse_x) << 2);
				doom_ev.data3 = ((sdl_mouse_y) << 2);
				// [TODO] Improve this ^

				D_PostEvent(&doom_ev);
				break;
			};

			default: break;
		};
	};
};

void I_StartTic(void) {
	if(!sdl_win)
		return;

	I_GetEvents();

	//SDL_WarpMouseInWindow(sdl_win, (SCREENWIDTH << WIN_SHIFT_FACTOR >> 1), (SCREENHEIGHT << WIN_SHIFT_FACTOR) >> 1);
};
