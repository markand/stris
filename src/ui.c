/*
 * ui.c -- user interface rendering
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "fonts/actionj.h"
#include "fonts/cartoon-relief.h"
#include "fonts/instruction.h"
#include "fonts/typography-ties.h"

#include "ui.h"
#include "util.h"
#include "stris.h"
#include "tex.h"

static struct tex *target;

SDL_Window *ui_win = NULL;
SDL_Renderer *ui_rdr = NULL;

#define CR(c) ((c >> 24) & 0xff)
#define CG(c) ((c >> 16) & 0xff)
#define CB(c) ((c >>  8) & 0xff)
#define CA(c) (c         & 0xff)
#define CHEX(r, g, b) ((uint32_t)r << 24 | (uint32_t)g << 16 | (uint32_t)b << 8 | 0xff)

#define PHY_W (UI_W * sconf.scale)
#define PHY_H (UI_H * sconf.scale)

static struct {
	int x;
	int y;
	int w;
	int h;
	int spent;
	uint32_t color;
} bg = {
	.w = UI_W / 10,
	.h = UI_W / 10,
	.color = UI_PALETTE_MENU_BG
};

static struct {
	const unsigned char *data;
	size_t datasz;
	int size;
	TTF_Font *font;
} fonts[] = {
	[UI_FONT_SPLASH] = {
		.data = assets_fonts_typography_ties,
		.datasz = sizeof (assets_fonts_typography_ties),
		.size = 58
	},
	[UI_FONT_TITLE] = {
		.data = assets_fonts_actionj,
		.datasz = sizeof (assets_fonts_actionj),
		.size = 80
	},
	[UI_FONT_MENU] = {
		.data = assets_fonts_cartoon_relief,
		.datasz = sizeof (assets_fonts_cartoon_relief),
		.size = 60
	},
	[UI_FONT_MENU_SMALL] = {
		.data = assets_fonts_instruction,
		.datasz = sizeof (assets_fonts_instruction),
		.size = 30
	},
	[UI_FONT_STATS] = {
		.data = assets_fonts_instruction,
		.datasz = sizeof (assets_fonts_instruction),
		.size = 18
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
init_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		fonts[i].font = load_font(fonts[i].data, fonts[i].datasz, fonts[i].size);
}

static inline void
finish_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		TTF_CloseFont(fonts[i].font);
}

static void
render(struct tex *t, enum ui_font f, uint32_t color, const char *fmt, va_list ap)
{
	char buf[128];
	SDL_Color c = { CR(color), CG(color), CB(color), CA(color) };
	SDL_Surface *sf;

	vsnprintf(buf, sizeof (buf), fmt, ap);

	if (!(sf = TTF_RenderUTF8_Blended(fonts[f].font, buf, c)))
		die("abort: %s\n", SDL_GetError());
	if (!(t->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (SDL_QueryTexture(t->handle, NULL, NULL, &t->w, &t->h) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_DestroySurface(sf);
}

static inline void
set_color(uint32_t color)
{
	SDL_SetRenderDrawColor(ui_rdr, CR(color), CG(color), CB(color), CA(color));
}

void
ui_init(void)
{
	int flags = IMG_INIT_PNG;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD) < 0)
		die("abort: %s\n", SDL_GetError());
	if (TTF_Init() < 0)
		die("abort: %s\n", SDL_GetError());
	if (IMG_Init(flags) != flags)
		die("abort: %s\n", SDL_GetError());
	if (SDL_CreateWindowAndRenderer(PHY_W, PHY_H, 0, &ui_win, &ui_rdr) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_SetRenderLogicalPresentation(ui_rdr, UI_W, UI_H,
	    SDL_LOGICAL_PRESENTATION_STRETCH, SDL_SCALEMODE_BEST);
	SDL_SetWindowTitle(ui_win, "STris");
	SDL_HideCursor();

	init_fonts();
}

void
ui_resize(void)
{
	SDL_SetWindowSize(ui_win, PHY_W, PHY_H);
}

void
ui_render(struct tex *t, enum ui_font f, uint32_t color, const char *fmt, ...)
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
ui_clear(uint32_t color)
{
	set_color(color);
	SDL_RenderClear(ui_rdr);
}

void
ui_update_background(uint32_t color, int ticks)
{
	int rcur, gcur, bcur;
	int rnxt, gnxt, bnxt;

	bg.spent += ticks;

	if (bg.spent >= 50) {
		bg.spent = 0;

		if (sconf.psychedelic) {
			bg.x--;
			bg.y--;
		} else
			bg.x = bg.y = 0;

		// Also update the color if we haven't reached yet.
		rcur = CR(bg.color); rnxt = CR(color);
		gcur = CG(bg.color); gnxt = CG(color);
		bcur = CB(bg.color); bnxt = CB(color);

		if (bg.color != color) {
			if (rcur != rnxt)
				rcur += (rcur < rnxt) ? 1 : -1;
			if (gcur != gnxt)
				gcur += (gcur < gnxt) ? 1 : -1;
			if (bcur != bnxt)
				bcur += (bcur < bnxt) ? 1 : -1;

			bg.color = CHEX(rcur, gcur, bcur);
		}
	}

	if (bg.x == -bg.w)
		bg.x = bg.y = 0;
}

void
ui_draw_background(void)
{
	int x, y, r, g, b;
	uint32_t color = bg.color;

	ui_draw_rect(color, 0, 0, UI_W, UI_H);

	// Darken color.
	r = clamp((long)CR(color) - 10, 0, 255);
	g = clamp((long)CG(color) - 10, 0, 255);
	b = clamp((long)CB(color) - 10, 0, 255);
	color = CHEX(r, g, b);

	for (int r = 0; r < 21; ++r) {
		for (int c = 0; c < 11; c += 2) {
			x = (bg.x + bg.w * c) + (r % 2 == 0 ? bg.w : 0);
			y = (bg.y + bg.h * r);
			ui_draw_rect(color, x, y, bg.w, bg.h);
		}
	}
}

void
ui_draw_line(uint32_t color, int x1, int y1, int x2, int y2)
{
	set_color(color);
	SDL_RenderLine(ui_rdr, x1, y1, x2, y2);
}

void
ui_draw_rect(uint32_t color, int x, int y, int w, int h)
{
	set_color(color);
	SDL_RenderFillRect(ui_rdr, &(const SDL_FRect){x, y, w, h});
}

void
ui_present(void)
{
	SDL_RenderPresent(ui_rdr);
}

struct tex *
ui_target(struct tex *tex)
{
	struct tex *old = target;

	SDL_SetRenderTarget(ui_rdr, tex ? tex->handle : NULL);
	target = tex;

	return old;
}

void
ui_finish(void)
{
	finish_fonts();

	SDL_DestroyRenderer(ui_rdr);
	SDL_DestroyWindow(ui_win);
	SDL_Quit();
}
