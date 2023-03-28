/*
 * tex.c -- texture management
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

#include <SDL.h>
#include <SDL_image.h>

#include <assert.h>
#include <string.h>

#include "tex.h"
#include "ui.h"
#include "util.h"

void
tex_new(struct tex *t, int w, int h)
{
	assert(t);

	if (!(t->handle = SDL_CreateTexture(ui_rdr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h)))
		die("abort: SDL_CreateTexture: %s\n", SDL_GetError());

	SDL_SetTextureBlendMode(t->handle, SDL_BLENDMODE_BLEND);
	t->w = w;
	t->h = h;
}

void
tex_load(struct tex *t, const void *data, size_t datasz)
{
	assert(t);

	SDL_Surface *sf;
	SDL_RWops *src;

	if (!(src = SDL_RWFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(sf = IMG_Load_RW(src, 1)))
		die("abort: %s\n", SDL_GetError());
	if (!(t->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (SDL_QueryTexture(t->handle, NULL, NULL, &t->w, &t->h) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_DestroySurface(sf);
}

void
tex_draw(struct tex *t, int x, int y)
{
	assert(t);
	assert(t->handle);

	SDL_RenderTexture(ui_rdr, t->handle, NULL, &(const SDL_FRect) {
		.x = x,
		.y = y,
		.w = t->w,
		.h = t->h
	});
}

void
tex_scale(struct tex *t, int x, int y, int w, int h)
{
	assert(t);
	assert(t->handle);

	const SDL_FRect rsrc = {
		.w = t->w,
		.h = t->h
	};
	const SDL_FRect rdst = {
		.x = x,
		.y = y,
		.w = w,
		.h = h
	};

	SDL_RenderTexture(ui_rdr, t->handle, &rsrc, &rdst);
}

void
tex_alpha(struct tex *t, int alpha)
{
	assert(t);
	assert(t->handle);

	SDL_SetTextureAlphaMod(t->handle, alpha);
}

void
tex_finish(struct tex *t)
{
	assert(t);

	if (t->handle)
		SDL_DestroyTexture(t->handle);

	memset(t, 0, sizeof (*t));
}
