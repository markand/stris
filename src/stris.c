/*
 * stris.c -- main stris file
 *
 * Copyright (c) 2011-2021 David Demelier <markand@malikania.fr>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <assert.h>

#include "sound.h"
#include "state-splash.h"
#include "state.h"
#include "stris.h"
#include "ui.h"

struct stris stris = {
	.audio = 1,
	.fullscreen = 1,
	.state = &state_splash
};

static int run = 1;

void
stris_switch(struct state *s)
{
	assert(s);

	stris.state_next = s;
}

int
main(int argc, char **argv)
{
	SDL_Event ev;
	Uint32 start, end, diff;
	Uint64 fstart, fend;
	float fspent;

	ui_init();
	sound_init();

	state_start(stris.state);

	start = end = SDL_GetTicks();

	while (run) {
		diff = end - start;
		start = SDL_GetTicks();
		fstart = SDL_GetPerformanceCounter();

		if (stris.state_next) {
			state_suspend(stris.state);
			state_start(stris.state_next);

			stris.state = stris.state_next;
			stris.state_next = NULL;
		}

		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				run = 0;
				break;
			default:
				break;
			}
		}

		state_update(stris.state, diff);
		state_draw(stris.state);
		fend = SDL_GetPerformanceCounter();

		fspent = (fend - fstart) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

		if (fspent < 16.666f)
			SDL_Delay(floor(16.666f - fspent));

		end = SDL_GetTicks();
	}

	ui_quit();

	return 0;
}
