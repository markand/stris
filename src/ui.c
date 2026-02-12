/*
 * ui.c -- user interface rendering
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
#include <stdarg.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "fonts/actionj.h"
#include "fonts/cartoon-relief.h"
#include "fonts/instruction.h"
#include "fonts/typography-ties.h"

#include "coroutine.h"
#include "node.h"
#include "stris.h"
#include "texture.h"
#include "ui.h"
#include "util.h"

#define PHY_W (UI_W * sconf.scale)
#define PHY_H (UI_H * sconf.scale)

static struct texture *target;

SDL_Window *ui_win = NULL;
SDL_Renderer *ui_rdr = NULL;

/* Grid background. */
static struct {
	uint32_t target;
	struct node background;
	struct coroutine updater;
} bg;

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
	SDL_IOStream *stream;
	TTF_Font *font;

	if (!(stream = SDL_IOFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(font = TTF_OpenFontIO(stream, 1, size)))
		die("abort: %s\n", SDL_GetError());

	return font;
}

static void
init_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		fonts[i].font = load_font(fonts[i].data, fonts[i].datasz, fonts[i].size);
}

static void
ui_bg_updater_entry(struct coroutine *)
{
	struct texture texture;
	int rcur, gcur, bcur;
	int rnxt, gnxt, bnxt;
	uint32_t color, darken;
	int w, h, x, y, r, g, b;

	/* Initial color. */
	bg.target = color = UI_PALETTE_MENU_BG;

	/* Size of individual square */
	w = h = UI_W / 10;

	/* Prepare background texture. */
	texture_init(&texture, UI_W, UI_H);
	node_wrap(&bg.background, &texture);

	x = y = 0;

	for (;;) {
		coroutine_sleep(50);

		if (sconf.psychedelic) {
			x--;
			y--;
		} else
			x = y = 0;

		/* Gradually reach the target color. */
		rcur = UI_COLOR_R(color); rnxt = UI_COLOR_R(bg.target);
		gcur = UI_COLOR_G(color); gnxt = UI_COLOR_G(bg.target);
		bcur = UI_COLOR_B(color); bnxt = UI_COLOR_B(bg.target);

		if (color != bg.target) {
			if (rcur != rnxt)
				rcur += (rcur < rnxt) ? 1 : -1;
			if (gcur != gnxt)
				gcur += (gcur < gnxt) ? 1 : -1;
			if (bcur != bnxt)
				bcur += (bcur < bnxt) ? 1 : -1;

			color = UI_COLOR(rcur, gcur, bcur, 0xff);
		}

		if (x == -w)
			x = y = 0;

		UI_BEGIN(bg.background.texture);
		ui_clear(color);

		/* Darken color for grid pattern. */
		r = clamp((long)UI_COLOR_R(color) - 10, 0, 255);
		g = clamp((long)UI_COLOR_G(color) - 10, 0, 255);
		b = clamp((long)UI_COLOR_B(color) - 10, 0, 255);
		darken = UI_COLOR(r, g, b, 0xff);

		for (int r = 0; r < 21; ++r)
			for (int c = 0; c < 11; c += 2)
				ui_draw_rect(darken, (x + w * c) + (r % 2 == 0 ? w : 0), (y + h * r), w, h);

		UI_END();
	}
}

static void
init_bg(void)
{
	bg.updater.entry = ui_bg_updater_entry;
	coroutine_init(&bg.updater);
}

static inline void
finish_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		TTF_CloseFont(fonts[i].font);
}

static void
render(struct texture *texture, enum ui_font f, uint32_t color, const char *fmt, va_list ap)
{
	char buf[128];
	SDL_Color c = {};
	SDL_Surface *sf;
	float w, h;

	c.r = UI_COLOR_R(color);
	c.g = UI_COLOR_G(color);
	c.b = UI_COLOR_B(color);
	c.a = UI_COLOR_A(color);

	vsnprintf(buf, sizeof (buf), fmt, ap);

	if (!(sf = TTF_RenderText_Blended(fonts[f].font, buf, strlen(buf), c)))
		die("abort: %s\n", SDL_GetError());
	if (!(texture->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (!SDL_GetTextureSize(texture->handle, &w, &h))
		die("abort: %s\n", SDL_GetError());

	texture->w = w;
	texture->h = h;

	SDL_DestroySurface(sf);
}

static inline void
set_color(uint32_t color)
{
	SDL_SetRenderDrawColor(ui_rdr,
	    UI_COLOR_R(color),
	    UI_COLOR_G(color),
	    UI_COLOR_B(color),
	    UI_COLOR_A(color)
	);
}

void
ui_init(void)
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
		die("abort: %s\n", SDL_GetError());
	if (!TTF_Init() || !MIX_Init())
		die("abort: %s\n", SDL_GetError());
	if (!SDL_CreateWindowAndRenderer("STris", PHY_W, PHY_H, 0, &ui_win, &ui_rdr))
		die("abort: %s\n", SDL_GetError());

	SDL_SetRenderLogicalPresentation(ui_rdr, UI_W, UI_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);
	SDL_SetWindowTitle(ui_win, "STris");
	SDL_HideCursor();

	init_fonts();
	init_bg();
}

void
ui_resize(void)
{
	SDL_SetWindowSize(ui_win, PHY_W, PHY_H);
}

void
ui_render(struct texture *texture, enum ui_font font, uint32_t color, const char *fmt, ...)
{
	assert(fmt);

	va_list ap;

	va_start(ap, fmt);
	render(texture, font, color, fmt, ap);
	va_end(ap);
}

void
ui_clip(enum ui_font f, int *w, int *h, const char *fmt, ...)
{
	assert(w);
	assert(h);
	assert(fmt);

	struct texture texture;
	va_list ap;

	va_start(ap, fmt);
	render(&texture, f, 0, fmt, ap);
	va_end(ap);

	*w = texture.w;
	*h = texture.h;

	texture_finish(&texture);
}

void
ui_clear(uint32_t color)
{
	set_color(color);
	SDL_RenderClear(ui_rdr);
}

void
ui_background_set(uint32_t color)
{
	bg.target = color;
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

struct texture *
ui_target(struct texture *texture)
{
	struct texture *old = target;

	SDL_SetRenderTarget(ui_rdr, texture ? texture->handle : NULL);
	target = texture;

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
