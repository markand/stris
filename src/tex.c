/*
 * tex.c -- texture management
 *
 * Copyright (c) 2011-2022 David Demelier <markand@malikania.fr>
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

#include <assert.h>
#include <string.h>

#include "tex.h"
#include "ui.h"

void
tex_draw(struct tex *t, int x, int y)
{
	assert(t);
	assert(t->handle);

	SDL_RenderCopy(ui_rdr, t->handle, NULL, &(const SDL_Rect) {
		.x = x,
		.y = y,
		.w = t->w,
		.h = t->h
	});
}

void
tex_finish(struct tex *t)
{
	assert(t);

	if (t->handle)
		SDL_DestroyTexture(t->handle);

	memset(t, 0, sizeof (*t));
}
