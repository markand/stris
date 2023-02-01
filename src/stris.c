/*
 * stris.c -- main stris file
 *
 * Copyright (c) 2011-2023 David Demelier <markand@malikania.fr>
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
#include <stdlib.h>
#include <time.h>

#include "joy.h"
#include "key.h"
#include "sound.h"
#include "state-dead.h"
#include "state-menu.h"
#include "state-mode.h"
#include "state-play.h"
#include "state-scores.h"
#include "state-settings.h"
#include "state-splash.h"
#include "state.h"
#include "stris.h"
#include "sys.h"
#include "ui.h"
#include "util.h"

static struct {
	int run;
	struct state *state;
	struct state *state_next;
} stris = {
	.run = 1,
#if defined(STRIS_NO_SPLASH)
	.state_next = &state_menu
#else
	.state_next = &state_splash
#endif
};

struct sconf sconf ={
	.sound = 1,
	.psychedelic = 1,
	.scale = 1
};

static struct state *states[] = {
	&state_dead,
	&state_menu,
	&state_mode,
	&state_play,
	&state_scores,
	&state_settings,
	&state_splash
};

static void
init(void)
{
	srand(time(NULL));

	sys_conf_read();
	ui_init();
	joy_init();

	if (sconf.sound)
		sound_init();

	for (size_t i = 0; i < LEN(states); ++i)
		state_init(states[i]);
}

static void
change(void)
{
	if (stris.state_next) {
		if (stris.state)
			state_suspend(stris.state);

		state_resume(stris.state_next);

		stris.state = stris.state_next;
		stris.state_next = NULL;
	}
}

static void
handle_controller_axis_motion(const SDL_ControllerAxisEvent *ev)
{
	// Axis generates lots of event so we need to stop taking care of them
	// unless we go back to the original state.
	static enum key joy_x;
	static enum key joy_y;

	int state = -1;
	enum key key = KEY_NONE;

	switch (ev->axis) {
	case SDL_CONTROLLER_AXIS_LEFTX:
		if (ev->value < -8000) {
			if (!joy_x) {
				state = 1;
				joy_x = key = KEY_LEFT;
			}
		} else if (ev->value > 8000) {
			if (!joy_x) {
				state = 1;
				joy_x = key = KEY_RIGHT;
			}
		} else {
			key = joy_x;
			joy_x = state = 0;
		}
		break;
	case SDL_CONTROLLER_AXIS_LEFTY:
		if (ev->value < -8000) {
			if (!joy_y) {
				state = 1;
				joy_y = key = KEY_UP;
			}
		} else if (ev->value > 8000) {
			if (!joy_y) {
				state = 1;
				joy_y = key = KEY_DOWN;
			}
		} else {
			key = joy_y;
			joy_y = state = 0;
		}
		break;
	default:
		break;
	}

	if (key)
		state_onkey(stris.state, key, state);
}

static void
handle_controller_button(const SDL_ControllerButtonEvent *ev)
{
	int state = ev->type == SDL_CONTROLLERBUTTONDOWN;
	enum key key = KEY_NONE;

	switch (ev->button) {
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		key = KEY_UP;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		key = KEY_RIGHT;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		key = KEY_DOWN;
		break;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		key = KEY_LEFT;
		break;
	case SDL_CONTROLLER_BUTTON_A:
		key = KEY_SELECT;
		break;
	case SDL_CONTROLLER_BUTTON_B:
		key = KEY_CANCEL;
		break;
	case SDL_CONTROLLER_BUTTON_X:
		key = KEY_DROP;
		break;
	default:
		break;
	}

	if (key)
		state_onkey(stris.state, key, state);
}

static void
handle_keyboard(const SDL_KeyboardEvent *ev)
{
	int state = ev->type == SDL_KEYDOWN;
	enum key key = KEY_NONE;

	switch (ev->keysym.scancode) {
	case SDL_SCANCODE_ESCAPE:
		key = KEY_CANCEL;
		break;
	case SDL_SCANCODE_RETURN:
		key = KEY_SELECT;
		break;
	case SDL_SCANCODE_UP:
		key = KEY_UP;
		break;
	case SDL_SCANCODE_RIGHT:
		key = KEY_RIGHT;
		break;
	case SDL_SCANCODE_DOWN:
		key = KEY_DOWN;
		break;
	case SDL_SCANCODE_LEFT:
		key = KEY_LEFT;
		break;
	case SDL_SCANCODE_SPACE:
		key = KEY_DROP;
		break;
	default:
		break;
	}

	if (key)
		state_onkey(stris.state, key, state);
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
		case SDL_CONTROLLERAXISMOTION:
			handle_controller_axis_motion(&ev.caxis);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			handle_controller_button(&ev.cbutton);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			handle_keyboard(&ev.key);
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
	for (size_t i = 0; i < LEN(states); ++i)
		state_init(states[i]);

	if (sconf.sound)
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
	(void)argc;
	(void)argv;

	init();
	loop();
	finish();

	return 0;
}
