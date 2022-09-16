/*
 * ui.c -- user interface rendering
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "fonts/actionj.h"
#include "fonts/cartoon-relief.h"
#include "fonts/dejavu-sans.h"
#include "fonts/typography-ties.h"

#include "ui.h"
#include "util.h"
#include "tex.h"

SDL_Window *ui_win = NULL;
SDL_Renderer *ui_rdr = NULL;

static struct {
	const unsigned char *data;
	size_t datasz;
	int size;
	TTF_Font *font;
} fonts[] = {
	[UI_FONT_SPLASH] = {
		.data = data_fonts_typography_ties,
		.datasz = sizeof (data_fonts_typography_ties),
		.size = 58
	},
	[UI_FONT_TITLE] = {
		.data = data_fonts_actionj,
		.datasz = sizeof (data_fonts_actionj),
		.size = 80
	},
	[UI_FONT_MENU] = {
		.data = data_fonts_cartoon_relief,
		.datasz = sizeof (data_fonts_cartoon_relief),
		.size = 60
	},
	[UI_FONT_MENU_SMALL] = {
		.data = data_fonts_dejavu_sans,
		.datasz = sizeof (data_fonts_dejavu_sans),
		.size = 20
	}
};

static TTF_Font *
load_font(const unsigned char *data, size_t datasz, int size)
{
	SDL_RWops *ops;
	TTF_Font *font;

	if (!(ops = SDL_RWFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(font = TTF_OpenFontRW(ops, 1, size)))
		die("abort: %s\n", SDL_GetError());

	return font;
}

static inline void
load_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		fonts[i].font = load_font(fonts[i].data, fonts[i].datasz, fonts[i].size);
}

static void
render(struct tex *t, enum ui_font f, enum ui_palette color, const char *fmt, va_list ap)
{
	char buf[128];
	SDL_Color c = {
		.r = (color >> 24) & 0xff,
		.g = (color >> 16) & 0xff,
		.b = (color >> 8)  & 0xff
	};
	SDL_Surface *sf;

	vsnprintf(buf, sizeof (buf), fmt, ap);

	if (!(sf = TTF_RenderUTF8_Blended(fonts[f].font, buf, c)))
		die("abort: %s\n", SDL_GetError());
	if (!(t->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (SDL_QueryTexture(t->handle, NULL, NULL, &t->w, &t->h) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_FreeSurface(sf);
}

void
ui_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		die("abort: %s\n", SDL_GetError());
	if (TTF_Init() < 0)
		die("abort: %s\n", SDL_GetError());
	if (SDL_CreateWindowAndRenderer(UI_W, UI_H, 0, &ui_win, &ui_rdr) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_SetWindowTitle(ui_win, "STris");

	load_fonts();
}

void
ui_render(struct tex *t, enum ui_font f, enum ui_palette color, const char *fmt, ...)
{
	assert(fmt);

	va_list ap;

	va_start(ap, fmt);
	render(t, f, color, fmt, ap);
	va_end(ap);
}

void
ui_clip(enum ui_font f, int *w, int *h, const char *fmt, ...)
{
	assert(w);
	assert(h);
	assert(fmt);

	struct tex tex;
	va_list ap;

	va_start(ap, fmt);
	render(&tex, f, 0, fmt, ap);
	va_end(ap);

	*w = tex.w;
	*h = tex.h;

	tex_finish(&tex);
}

void
ui_clear(enum ui_palette color)
{
	SDL_SetRenderDrawColor(ui_rdr,
	    (color >> 24) & 0xff,
	    (color >> 16) & 0xff,
	    (color >> 8)  & 0xff,
	    0xff
	);
	SDL_RenderClear(ui_rdr);
}

void
ui_present(void)
{
	SDL_RenderPresent(ui_rdr);
}

void
ui_finish(void)
{
	SDL_DestroyRenderer(ui_rdr);
	SDL_DestroyWindow(ui_win);
	SDL_Quit();
}
