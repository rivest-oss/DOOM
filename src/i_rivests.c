#ifndef __I_RIVESTS__C__
#define __I_RIVESTS__C__

//-----------------------------------------------------------------------------
// 
// DESCRIPTION:
// ---> Rivest's thingies.
//
//-----------------------------------------------------------------------------

#include "i_rivests.h"
#include "s_sound.h"

#include "hu_stuff.h"

extern void M_WriteText(int x, int y, char *string);

const char *r_secret_text = NULL;
int r_secret_text_tick = -1;

void I_Rivests_SecretRevealed(player_t *player) {
	r_secret_text = "A secret is revealed!";
	r_secret_text_tick = ((TICRATE << 1) + TICRATE);

	// [TODO] Play sound...
};

void I_Rivests_Tick_OnVisualBeforeMenu(void) {
	if(r_secret_text != NULL) {
		M_WriteText((SCREENWIDTH >> 2), (SCREENHEIGHT >> 1) - 8, (char *)r_secret_text);

		r_secret_text_tick--;

		if(r_secret_text_tick <= 0) {
			r_secret_text = NULL;
			r_secret_text_tick = -1;
		}
	}
};

#endif
