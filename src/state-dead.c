/*
 * state-dead.c -- game over state
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

#include "key.h"
#include "state-dead.h"
#include "state-menu.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"

static struct tex tex[2];
static int x;
static int y;

static void
init(void)
{
	ui_render(&tex[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, "game over");
	ui_render(&tex[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, "game over");
	x = (UI_W - tex[0].w) / 2;
	y = (UI_H - tex[0].h) / 2;
}

static void
onkey(enum key key)
{
	(void)key;

	stris_switch(&state_menu);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	tex_draw(&tex[1], x + 1, y + 1);
	tex_draw(&tex[0], x, y);
	ui_present();
}

static void
finish(void)
{
}

struct state state_dead = {
	.init = init,
	.draw = draw,
	.onkey = onkey,
	.finish = finish
};
