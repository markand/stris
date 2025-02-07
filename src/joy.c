/*
 * joy.c -- joystick handling
 *
 * Copyright (c) 2011-2025 David Demelier <markand@malikania.fr>
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

#include <stdio.h>

#include <SDL.h>

#include "gamecontrollerdb.h"
#include "joy.h"

static SDL_Gamepad *ctl;
static SDL_JoystickID *ids;

void
joy_init(void)
{
	int count = 0;

	if (SDL_AddGamepadMapping((const char *)assets_gamecontrollerdb) < 0) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return;
	}
	if (!(ids = SDL_GetJoysticks(&count))) {
		fprintf(stderr, "%s\n", SDL_GetError());
		return;
	}

	for (int i = 0; i < count; ++i) {
		if (!SDL_IsGamepad(ids[i]))
			continue;

		if ((ctl = SDL_OpenGamepad(i)))
			break;
		else
			fprintf(stderr, "controller %d: %s\n", i, SDL_GetError());
	}
}

void
joy_finish(void)
{
	SDL_CloseGamepad(ctl);
	ctl = NULL;

	SDL_free(ids);
	ids = NULL;
}
