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

/**
 * \file ui.h
 * \brief User interface rendering.
 */

#include <stdint.h>

/**
 * Store the current texture in a temporary variable and switch to the
 * rendering target provided as argument.
 *
 * \param Texture the texture to use
 */
#define UI_BEGIN(Texture)                                                       \
do {                                                                            \
        struct texture *ui__ctx;                                                \
                                                                                \
        ui__ctx = ui_target((Texture));

/**
 * Pop the current rendering texture and restore the previous one.
 */
#define UI_END()                                                                \
        ui_target(ui__ctx);                                                     \
} while (0)

/**
 * Window width.
 */
#define UI_W 360

/**
 * Window height.
 */
#define UI_H 640

/**
 * Expand red part of `Color`.
 */
#define UI_COLOR_R(Color) (((Color) >> 24) & 0xff)

/**
 * Expand green part of `Color`.
 */
#define UI_COLOR_G(Color) (((Color) >> 16) & 0xff)

/**
 * Expand blue part of `Color`.
 */
#define UI_COLOR_B(Color) (((Color) >>  8) & 0xff)

/**
 * Expand alpha part of `Color`.
 */
#define UI_COLOR_A(Color) (((Color) >>  0) & 0xff)

/**
 * Expand to a color using all four RGBA components.
 */
#define UI_COLOR(R, G, B, A) (                                                  \
        ((uint32_t)(R) << 24) & (uint32_t)0xff000000 |                          \
        ((uint32_t)(G) << 16) & (uint32_t)0x00ff0000 |                          \
        ((uint32_t)(B) <<  8) & (uint32_t)0x0000ff00 |                          \
        ((uint32_t)(A) <<  0) & (uint32_t)0x000000ff                            \
)

struct texture;

/**
 * \enum ui_palette
 * \brief Predefined color palette.
 *
 * Based on https://lospec.com/palette-list/give-me-grey-52.
 */
enum ui_palette : uint32_t {
	UI_PALETTE_SPLASH_BG = 0x3f4a69ff,
	UI_PALETTE_FG = 0xffffffff,
	UI_PALETTE_MENU_BG = 0xb3b9d1ff,
	UI_PALETTE_MENU_LOW = 0xa6a6a6ff,
	UI_PALETTE_MENU_HIGH = 0xd87823ff,
	UI_PALETTE_SHADOW = 0x0c0101ff,
	UI_PALETTE_BORDER = 0x666655ff
};

/**
 * \enum ui_font
 * \brief Font atlas index.
 */
enum ui_font {
	UI_FONT_SPLASH,         /*!< splash screen font */
	UI_FONT_TITLE,          /*!< main menu title font */
	UI_FONT_MENU,           /*!< main menu list font */
	UI_FONT_MENU_SMALL,     /*!< alternative menu list font */
	UI_FONT_STATS           /*!< menu for stats/scores */
};

/**
 * Initialize user interface.
 */
void
ui_init(void);

void
ui_resize(void);

void
ui_render(struct texture *, enum ui_font, uint32_t, const char *, ...);

void
ui_render_shadowed(struct texture *, enum ui_font, uint32_t, const char *, ...);

void
ui_clip(enum ui_font font, unsigned int *w, unsigned int *h, const char *fmt, ...);

void
ui_clear(uint32_t);

/**
 * Change the grid background color.
 *
 * This function is not immediate as the color changes gradually.
 */
void
ui_background_set(uint32_t color);

void
ui_draw_line(uint32_t, int, int, int, int);

void
ui_draw_rect(uint32_t, int, int, int, int);

void
ui_present(void);

struct texture *
ui_target(struct texture *);

void
ui_finish(void);

#endif /* !STRIS_UI_H */
