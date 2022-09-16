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

#include "key.h"
#include "sound.h"
#include "state-splash.h"
#include "state.h"
#include "stris.h"
#include "ui.h"

static struct {
	int run;
	struct state *state;
	struct state *state_next;
} stris = {
	.run = 1,
	.state = &state_splash
};

static void
init(void)
{
	ui_init();
	sound_init();
	state_start(stris.state);
}

static void
change(void)
{
	if (stris.state_next) {
		state_suspend(stris.state);
		state_start(stris.state_next);

		stris.state = stris.state_next;
		stris.state_next = NULL;
	}
}

static void
handle(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			stris.run = 0;
			break;
		case SDL_KEYDOWN:
			// TODO: Add options mapping here.
			switch (ev.key.keysym.scancode) {
			case SDL_SCANCODE_RETURN:
				state_onkey(stris.state, KEY_SELECT);
				break;
			case SDL_SCANCODE_UP:
				state_onkey(stris.state, KEY_UP);
				break;
			case SDL_SCANCODE_RIGHT:
				state_onkey(stris.state, KEY_RIGHT);
				break;
			case SDL_SCANCODE_DOWN:
				state_onkey(stris.state, KEY_DOWN);
				break;
			case SDL_SCANCODE_LEFT:
				state_onkey(stris.state, KEY_LEFT);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

static inline void
update(int diff)
{
	state_update(stris.state, diff);
}

static inline void
draw(void)
{
	state_draw(stris.state);
}

static void
loop(void)
{
	Uint32 start, end, diff;
	Uint64 fstart, fend;
	float fspent;

	start = end = SDL_GetTicks();

	while (stris.run) {
		diff = end - start;
		start = SDL_GetTicks();
		fstart = SDL_GetPerformanceCounter();

		change();
		handle();
		update(diff);
		draw();

		fend = SDL_GetPerformanceCounter();
		fspent = (fend - fstart) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

		if (fspent < 16.666f)
			SDL_Delay(floor(16.666f - fspent));

		end = SDL_GetTicks();
	}
}

static void
finish(void)
{
	sound_finish();
	ui_finish();
}

void
stris_switch(struct state *s)
{
	assert(s);

	stris.state_next = s;
}

void
stris_quit(void)
{
	stris.run = 0;
}

int
main(int argc, char **argv)
{
	init();
	loop();
	finish();

	return 0;
}
