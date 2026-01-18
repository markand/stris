/*
 * ui.h -- user interface rendering
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

#ifndef STRIS_UI_H
#define STRIS_UI_H

#include <stdint.h>

#include <SDL3/SDL.h>

#define UI_W 360
#define UI_H 640

// https://lospec.com/palette-list/give-me-grey-52
#define UI_PALETTE_SPLASH_BG    0x3f4a69ff
#define UI_PALETTE_FG           0xffffffff
#define UI_PALETTE_MENU_BG      0xb3b9d1ff
#define UI_PALETTE_MENU_SEL     0xd87823ff
#define UI_PALETTE_SHADOW       0x0c0101ff
#define UI_PALETTE_BORDER       0x666655ff

enum ui_font {
	UI_FONT_SPLASH,
	UI_FONT_TITLE,
	UI_FONT_MENU,
	UI_FONT_MENU_SMALL,
	UI_FONT_STATS
};

struct tex;

/* Global window and its renderer. */
extern SDL_Window *ui_win;
extern SDL_Renderer *ui_rdr;

void
ui_init(void);

void
ui_resize(void);

void
ui_render(struct tex *, enum ui_font, uint32_t, const char *, ...);

void
ui_clip(enum ui_font, int *, int *, const char *, ...);

void
ui_clear(uint32_t);

void
ui_update_background(uint32_t, int);

void
ui_draw_background(void);

void
ui_draw_line(uint32_t, int, int, int, int);

void
ui_draw_rect(uint32_t, int, int, int, int);

void
ui_present(void);

struct tex *
ui_target(struct tex *);

void
ui_finish(void);

#endif /* !STRIS_UI_H */
