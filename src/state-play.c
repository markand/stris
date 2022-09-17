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

#include "img/block1.h"
#include "img/block2.h"
#include "img/block3.h"
#include "img/block4.h"
#include "img/block5.h"
#include "img/block6.h"
#include "img/block7.h"
#include "img/block8.h"

#include "board.h"
#include "key.h"
#include "shape.h"
#include "sound.h"
#include "state-menu.h"
#include "state-play.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

#define SHAPE_TEX(d) { .data = d, sizeof (d) }

static struct {
	struct tex tex[2];
	int x;
	int y;
	int enable;
} pause;

// Box geometry for stats.
static struct {
	int x[2];
	int y[2];
} statsgeo;

static struct {
	int level;
	int score;
	struct shape shape;
	struct shape shape_next;
} game;

static struct {
	struct tex tex;
	const void *data;
	size_t datasz;
} shapes[SHAPE_LAST] = {
	[SHAPE_O] = SHAPE_TEX(data_img_block5),
	[SHAPE_I] = SHAPE_TEX(data_img_block7),
	[SHAPE_S] = SHAPE_TEX(data_img_block2),
	[SHAPE_Z] = SHAPE_TEX(data_img_block4),
	[SHAPE_L] = SHAPE_TEX(data_img_block3),
	[SHAPE_J] = SHAPE_TEX(data_img_block1),
	[SHAPE_T] = SHAPE_TEX(data_img_block6)
};

static struct {
	int x;
	int y;
	int w;
	int h;
	int cw;
	int ch;
	Board b;
} board;

static void
init_title(void)
{
	// Prepare pause label.
	ui_render(&pause.tex[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, "pause");
	ui_render(&pause.tex[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, "pause");
	pause.x = (UI_W - pause.tex[0].w) / 2;
	pause.y = (UI_H - pause.tex[0].h) / 2;
}

static void
init_stats(void)
{
	int h, tw, th;

	// Compute position for stats labels.
	statsgeo.x[0] = statsgeo.x[1] = UI_W / 9;

	// Bounding box with some paddings removed.
	h = (UI_H / 16 * 2) - 10;

	// We need to compute text height.
	ui_clip(UI_FONT_STATS, &tw, &th, "lilou");
	h -= th * 2;
	h /= 3;

	statsgeo.y[0] = 5 + h;
	statsgeo.y[1] = statsgeo.y[0] + th + h;
}

static void
init_blocks(void)
{
	// Load all blocks.
	for (size_t i = 0; i < LEN(shapes); ++i)
		tex_load(&shapes[i].tex, shapes[i].data, shapes[i].datasz);
}

static void
init_board(void)
{
	// Board geometry.
	board.x = (UI_W / 9);
	board.y = (UI_H * 2)  / 16;
	board.w = (UI_W * 7)  / 9;
	board.h = (UI_H * 13) / 16;
	board.cw = shapes[0].tex.w;
	board.ch = shapes[0].tex.h;
}

static void
init_game(void)
{
	shape_select(&game.shape, SHAPE_RANDOM);
	shape_select(&game.shape_next, SHAPE_RANDOM);
	game.shape.x = 3;
	board_set(board.b, &game.shape);
}

static void
init(void)
{
	init_title();
	init_stats();
	init_blocks();
	init_board();
	init_game();
}

static void
resume(void)
{
	pause.enable = 0;
	game.level = 1;
	game.score = 0;
}

static int
can_move(int dx, int dy)
{
	struct shape shape = game.shape;

	// Try to put in the new place.
	shape.x += dx;
	shape.y += dy;

	return board_check(board.b, &shape);
}

static void
move(int dx, int dy)
{
	(void)dx;
	(void)dy;

	// First, remove the current shape to avoid collision with it.
	board_unset(board.b, &game.shape);

	if (can_move(dx, dy)) {
		sound_play(SOUND_MOVE);
		game.shape.x += dx;
		game.shape.y += dy;
	}

	board_set(board.b, &game.shape);
}

static void
drop(void)
{
	board_unset(board.b, &game.shape);

	while (can_move(0, 1))
		game.shape.y += 1;

	board_set(board.b, &game.shape);
	sound_play(SOUND_DROP);
}

static void
onkey(enum key key)
{
	switch (key) {
	case KEY_CANCEL:
		// Already in pause, escape to main menu then.
		if (pause.enable)
			stris_switch(&state_menu);
		else
			pause.enable = 1;
		break;
	case KEY_SELECT:
		pause.enable = 0;
		sound_play(SOUND_ROTATE);
		break;
	case KEY_RIGHT:
		move(1, 0);
		break;
	case KEY_DOWN:
		move(0, 1);
		break;
	case KEY_LEFT:
		move(-1, 0);
		break;
	case KEY_DROP:
		drop();
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
	(void)ticks;
}

static void
draw_stat(int row, const char *fmt, ...)
{
	va_list ap;
	char buf[128];
	struct tex tex[2];

	va_start(ap, fmt);
	vsnprintf(buf, sizeof (buf), fmt, ap);
	va_end(ap);

	ui_render(&tex[0], UI_FONT_STATS, UI_PALETTE_FG, "%s", buf);
	ui_render(&tex[1], UI_FONT_STATS, UI_PALETTE_SHADOW, "%s", buf);

	tex_draw(&tex[1], statsgeo.x[row] + 1, statsgeo.y[row] + 1);
	tex_draw(&tex[0], statsgeo.x[row], statsgeo.y[row]);
	tex_finish(&tex[0]);
	tex_finish(&tex[1]);
}

static void
draw_stats(void)
{
	draw_stat(0, "level %d", game.level);
	draw_stat(1, "score %d", game.score);
}

static void
draw_borders(void)
{
	// -
	ui_draw_line(UI_PALETTE_BORDER,
	    board.x - 1,
	    board.y - 1,
	    board.x + board.w + 1,
	    board.y - 1);
	ui_draw_line(UI_PALETTE_BORDER,
	    board.x - 1,
	    board.y + board.h + 1,
	    board.x + board.w + 1,
	    board.y + board.h + 1);

	// |
	ui_draw_line(UI_PALETTE_BORDER,
	    board.x - 1,
	    board.y - 1,
	    board.x - 1,
	    board.y + board.h + 1);
	ui_draw_line(UI_PALETTE_BORDER,
	    board.x + board.w + 1,
	    board.y - 1,
	    board.x + board.w + 1,
	    board.y + board.h + 1);
}

static void
draw_board(void)
{
	int s;

	for (int r = 0; r < BOARD_H; ++r) {
		for (int c = 0; c < BOARD_W; ++c) {
			if (!(s = board.b[r][c]))
				continue;

			tex_draw(&shapes[s].tex,
			    board.x + (c * board.cw),
			    board.y + (r * board.ch));
		}
	}
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);

	if (pause.enable) {
		tex_draw(&pause.tex[1], pause.x + 1, pause.y + 1);
		tex_draw(&pause.tex[0], pause.x, pause.y);
	} else {
		draw_stats();
		draw_borders();
		draw_board();
	}

	ui_present();
}

static void
finish(void)
{
	tex_finish(&pause.tex[0]);
	tex_finish(&pause.tex[1]);

	for (size_t i = 0; i < LEN(shapes); ++i)
		tex_finish(&shapes[i].tex);
}

struct state state_play = {
	.init = init,
	.resume = resume,
	.update = update,
	.draw = draw,
	.onkey = onkey,
	.finish = finish
};
