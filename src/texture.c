/*
 * tex.c -- texture management
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

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <assert.h>
#include <string.h>

#include "texture.h"
#include "ui.h"
#include "util.h"

/* private in ui.c */
extern SDL_Renderer *ui_rdr;

void
texture_init(struct texture *texture, unsigned int w, unsigned int h)
{
	assert(texture);

	if (!(texture->handle = SDL_CreateTexture(ui_rdr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h)))
		die("abort: SDL_CreateTexture: %s\n", SDL_GetError());

	SDL_SetTextureBlendMode(texture->handle, SDL_BLENDMODE_BLEND);
	texture->w = w;
	texture->h = h;
}

void
texture_load(struct texture *texture, const void *data, size_t datasz)
{
	assert(texture);

	SDL_Surface *sf;
	SDL_IOStream *src;
	float w, h;

	if (!(src = SDL_IOFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(sf = IMG_Load_IO(src, 1)))
		die("abort: %s\n", SDL_GetError());
	if (!(texture->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (!SDL_GetTextureSize(texture->handle, &w, &h))
		die("abort: %s\n", SDL_GetError());

	texture->w = w;
	texture->h = h;

	SDL_DestroySurface(sf);
}

void
texture_render(struct texture *texture, int x, int y)
{
	assert(texture);
	assert(texture->handle);

	SDL_RenderTexture(ui_rdr, texture->handle, NULL, &(const SDL_FRect) {
		.x = x,
		.y = y,
		.w = texture->w,
		.h = texture->h
	});
}

void
texture_scale(struct texture *texture, int x, int y, unsigned int w, unsigned int h)
{
	assert(texture);
	assert(texture->handle);

	const SDL_FRect rsrc = {
		.w = texture->w,
		.h = texture->h
	};
	const SDL_FRect rdst = {
		.x = x,
		.y = y,
		.w = w,
		.h = h
	};

	SDL_RenderTexture(ui_rdr, texture->handle, &rsrc, &rdst);
}

void
texture_alpha(struct texture *texture, unsigned int alpha)
{
	assert(texture);
	assert(texture->handle);

	SDL_SetTextureAlphaMod(texture->handle, alpha);
}

static inline void
texture_colorize(struct texture *texture, int blend, uint32_t color)
{
	uint8_t r, g, b;

	if (!SDL_SetTextureBlendMode(texture->handle, blend))
		die("SDL_SetTextureBlendMode: %s\n", SDL_GetError());

	if (blend != SDL_BLENDMODE_NONE) {
		r = UI_COLOR_R(color);
		g = UI_COLOR_G(color);
		b = UI_COLOR_B(color);

		if (!SDL_SetTextureColorMod(texture->handle, r, g, b))
			die("SDL_SetTextureColorMod: %s\n", SDL_GetError());
	}
}

void
texture_color_add(struct texture *texture, uint32_t color)
{
	assert(texture);
	assert(texture->handle);

	texture_colorize(texture, SDL_BLENDMODE_ADD, color);
}

void
texture_color_blend(struct texture *texture, uint32_t color)
{
	assert(texture);
	assert(texture->handle);

	texture_colorize(texture, SDL_BLENDMODE_BLEND, color);
}

void
texture_color_multiply(struct texture *texture, uint32_t color)
{
	assert(texture);
	assert(texture->handle);

	texture_colorize(texture, SDL_BLENDMODE_MUL, color);
}

void
texture_color_none(struct texture *texture)
{
	assert(texture);
	assert(texture->handle);

	texture_colorize(texture, SDL_BLENDMODE_NONE, 0);
}

void
texture_finish(struct texture *texture)
{
	assert(texture);

	if (texture->handle)
		SDL_DestroyTexture(texture->handle);

	texture->handle = NULL;
	texture->w = texture->h = 0;
}
