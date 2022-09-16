/*
 * state-play.c -- game state
 *
 * Copyright (c) 2011-2021 David Demelier <markand@malikania.fr>
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

#include "board.h"
#include "key.h"
#include "state-menu.h"
#include "state-play.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"

static struct {
	int paused;
	struct tex pausetex;
	Board board;
} game;

static void
init(void)
{
	ui_render(&game.pausetex, UI_FONT_MENU_SMALL, UI_PALETTE_FG, "pause");
}

static void
resume(void)
{
	game.paused = 0;
}

static void
onkey(enum key key)
{
	switch (key) {
	case KEY_CANCEL:
		// Already in pause, escape to main menu then.
		if (game.paused)
			stris_switch(&state_menu);
		else
			game.paused = 1;
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
}

static void
draw(void)
{
	ui_clear(0);

	if (game.paused) {
		tex_draw(&game.pausetex, 0, 0);
	} else {
		puts("draw normal...");
	}

	ui_present();
}

static void
finish(void)
{
	tex_finish(&game.pausetex);
}

struct state state_play = {
	.init = init,
	.resume = resume,
	.update = update,
	.draw = draw,
	.onkey = onkey,
	.finish = finish
};
