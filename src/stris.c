/*
 * stris.c -- main stris file
 *
 * Copyright (c) 2011-2026 David Demelier <markand@malikania.fr>
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "coroutine.h"
#include "joy.h"
#include "node.h"
#include "sound.h"
#include "state-splash.h"
#include "stris.h"
#include "sys.h"
#include "ui.h"
#include "util.h"

struct sconf sconf = {
	.sound = 0,
	.psychedelic = 1,
	.scale = 1
};

struct stris stris = {
	.run = 1
};

static void
handle_controller_axis_motion(const SDL_GamepadAxisEvent *ev)
{
	/*
	 * Axis generates lots of event so we need to stop taking care of them
	 * unless we go back to the original state.
	 */
	static enum key joy_x;
	static enum key joy_y;

	int state = -1;
	enum key key = KEY_NONE;

	switch (ev->axis) {
	case SDL_GAMEPAD_AXIS_LEFTX:
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
	case SDL_GAMEPAD_AXIS_LEFTY:
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

	if (state)
		stris.keys |= key;
	else
		stris.keys &= ~key;
}

static void
handle_controller_button(const SDL_GamepadButtonEvent *ev)
{
	int state = ev->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN;
	enum key key = KEY_NONE;

	switch (ev->button) {
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
		key = KEY_UP;
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
		key = KEY_RIGHT;
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		key = KEY_DOWN;
		break;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
		key = KEY_LEFT;
		break;
	case SDL_GAMEPAD_BUTTON_SOUTH:
		key = KEY_SELECT;
		break;
	case SDL_GAMEPAD_BUTTON_EAST:
		key = KEY_CANCEL;
		break;
	case SDL_GAMEPAD_BUTTON_WEST:
		key = KEY_DROP;
		break;
	default:
		break;
	}

	if (state)
		stris.keys |= key;
	else
		stris.keys &= ~key;
}

static void
handle_keyboard(const SDL_KeyboardEvent *ev)
{
	int state = ev->type == SDL_EVENT_KEY_DOWN;
	enum key key = KEY_NONE;

	switch (ev->scancode) {
	case SDL_SCANCODE_ESCAPE:
		key = KEY_CANCEL;
		break;
	case SDL_SCANCODE_RETURN:
		key = KEY_SELECT;
		break;
	case SDL_SCANCODE_UP:
	case SDL_SCANCODE_K:
		key = KEY_UP;
		break;
	case SDL_SCANCODE_RIGHT:
	case SDL_SCANCODE_L:
		key = KEY_RIGHT;
		break;
	case SDL_SCANCODE_DOWN:
	case SDL_SCANCODE_J:
		key = KEY_DOWN;
		break;
	case SDL_SCANCODE_LEFT:
	case SDL_SCANCODE_H:
		key = KEY_LEFT;
		break;
	case SDL_SCANCODE_SPACE:
		key = KEY_DROP;
		break;
	default:
		break;
	}

	if (state)
		stris.keys |= key;
	else
		stris.keys &= ~key;
}

enum key
stris_pressed(void)
{
	enum key current, diff;

	current = stris.keys;

	/* Yield until only a new bit is pressent in stris.keys */
	while ((current | stris.keys) == current) {
		current = stris.keys;
		coroutine_yield();
	}

	diff = current ^ stris.keys;

	return diff;
}

void
stris_quit(void)
{
	stris.run = 0;
}

SDL_AppResult
SDL_AppInit(void **, int, char **)
{
	srand(time(NULL));

	SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60.0");
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	sys_conf_read();
	ui_init();
	joy_init();

	if (sconf.sound)
		sound_init();

	splash_run();

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppIterate(void *)
{
	if (!stris.run)
		return SDL_APP_SUCCESS;

	for (size_t i = 0; i < LEN(stris.coroutines); ++i) {
		if (!stris.coroutines[i] || stris.coroutines[i]->pause)
			continue;

		if (coroutine_resume(stris.coroutines[i], 16))
			coroutine_finish(stris.coroutines[i]);
	}

	ui_clear(0xffffffff);

	for (size_t i = 0; i < LEN(stris.nodes); ++i)
		if (stris.nodes[i])
			node_render(stris.nodes[i]);

	ui_present();

	return SDL_APP_CONTINUE;
}

SDL_AppResult
SDL_AppEvent(void *, SDL_Event *ev)
{
	switch (ev->type) {
	case SDL_EVENT_QUIT:
		return SDL_APP_SUCCESS;
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
		handle_controller_axis_motion(&ev->gaxis);
		break;
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
		handle_controller_button(&ev->gbutton);
		break;
	case SDL_EVENT_KEY_DOWN:
	case SDL_EVENT_KEY_UP:
		handle_keyboard(&ev->key);
		break;
	default:
		break;
	}

	return SDL_APP_CONTINUE;
}

void
SDL_AppQuit(void *, SDL_AppResult)
{
	if (sconf.sound)
		sound_finish();

	ui_finish();
}
