/*
 * ui.h -- user interface rendering
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

#ifndef STRIS_UI_H
#define STRIS_UI_H

#include <SDL.h>

#define UI_W 360
#define UI_H 640

enum ui_font {
	UI_FONT_SPLASH,
	UI_FONT_TITLE,
	UI_FONT_MENU,
	UI_FONT_MENU_SMALL,
	UI_FONT_STATS
};

enum ui_palette : unsigned long {
	UI_PALETTE_SPLASH_BG    = 0x242b4aff,
	UI_PALETTE_FG           = 0xdfededff,
	UI_PALETTE_MENU_BG      = 0xa7b8c2ff,
	UI_PALETTE_MENU_SEL     = 0xff8766ff,
	UI_PALETTE_SHADOW       = 0x372840ff,
	UI_PALETTE_BORDER       = 0x242b4aff,

	// https://lospec.com/palette-list/aliengarden-32
	UI_PALETTE_LEVEL1       = 0x33984bff,
	UI_PALETTE_LEVEL2       = 0x1e6f50ff,
	UI_PALETTE_LEVEL3       = 0x0098dcff,
	UI_PALETTE_LEVEL4       = 0x0069aaff,
	UI_PALETTE_LEVEL5       = 0xee8201ff,
	UI_PALETTE_LEVEL6       = 0xc1460fff,
	UI_PALETTE_LEVEL7       = 0x801224ff,
	UI_PALETTE_LEVEL8       = 0x590924ff,
	UI_PALETTE_LEVEL9       = 0x2a2f4eff,
	UI_PALETTE_LEVEL10      = 0x1a1932ff
};

struct tex;

/* Global window and its renderer. */
extern SDL_Window *ui_win;
extern SDL_Renderer *ui_rdr;

void
ui_init(void);

void
ui_render(struct tex *, enum ui_font, enum ui_palette, const char *, ...);

void
ui_clip(enum ui_font, int *, int *, const char *, ...);

void
ui_clear(enum ui_palette);

void
ui_update_background(enum ui_palette, int);

void
ui_draw_background(void);

void
ui_draw_line(enum ui_palette, int, int, int, int);

void
ui_draw_rect(enum ui_palette, int, int, int, int);

void
ui_present(void);

void
ui_finish(void);

#endif /* !STRIS_UI_H */
