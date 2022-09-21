/*
 * state-play.c -- game state
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

#include "img/block1.h"
#include "img/block2.h"
#include "img/block3.h"
#include "img/block4.h"
#include "img/block5.h"
#include "img/block6.h"
#include "img/block7.h"
#include "img/block8.h"
#include "img/block9.h"
#include "img/block10.h"

#include "board.h"
#include "key.h"
#include "shape.h"
#include "sound.h"
#include "state-dead.h"
#include "state-menu.h"
#include "state-play.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

#define ANIM_DELAY 1000
#define ANIM_BLINK 100

#define FALLRATE_INIT 900
#define FALLRATE_DECR 74

#define SHAPE_TEX(d) { .data = d, sizeof (d) }

static struct {
	int lines;
	int spent;
	int blink;
	int blinkdelay;
} anim;

static struct {
	struct tex tex[2];
	int enable;
} pause;

static struct {
	int lines;
	int fallrate;
	int delay;
	enum shape_kind rand;
	struct shape shape;
	struct shape shape_next;
	Board board;
} game;

static struct {
	struct {
		int x, y, w, h;
	} header;

	struct {
		int x, y;
	} pause;

	struct {
		int x, y, s;
	} shape;

	struct {
		int x, y;
	} stats[2];

	struct {
		int x, y, w, h;
		int cw, ch;
	} board;
} geo;

static struct {
	struct tex tex;
	const void *data;
	size_t datasz;
} shapes[] = {
	[SHAPE_O] = SHAPE_TEX(data_img_block5),
	[SHAPE_I] = SHAPE_TEX(data_img_block6),
	[SHAPE_S] = SHAPE_TEX(data_img_block2),
	[SHAPE_Z] = SHAPE_TEX(data_img_block4),
	[SHAPE_L] = SHAPE_TEX(data_img_block1),
	[SHAPE_J] = SHAPE_TEX(data_img_block8),
	[SHAPE_T] = SHAPE_TEX(data_img_block3),
	[SHAPE_C] = SHAPE_TEX(data_img_block9),
	[SHAPE_X] = SHAPE_TEX(data_img_block10)
};

// https://lospec.com/palette-list/aliengarden-32
static const unsigned long ramp[10] = {
	0x33984bff,
	0x1e6f50ff,
	0x0098dcff,
	0x0069aaff,
	0xee8201ff,
	0xc1460fff,
	0x801224ff,
	0x590924ff,
	0x2a2f4eff,
	0x1a1932ff
};

static struct tex boardbg;

static void
init_pause(void)
{
	// Prepare pause label.
	ui_render(&pause.tex[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, "pause");
	ui_render(&pause.tex[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, "pause");
	geo.pause.x = (UI_W - pause.tex[0].w) / 2;
	geo.pause.y = (UI_H - pause.tex[0].h) / 2;
}

static void
init_header(void)
{
	// Bounding box with some paddings removed.
	geo.header.x = (UI_W / 9);
	geo.header.y = 10;
	geo.header.w = (UI_W * 7)  / 9;
	geo.header.h = (UI_H / 16 * 2) - 10;

	// Bounding box for next shape.
	geo.shape.s = geo.header.h;
	geo.shape.x = geo.header.x + geo.header.w - geo.shape.s;
	geo.shape.y = geo.header.y;
}

static void
init_stats(void)
{
	int h, tw, th;

	// Compute position for stats labels.
	geo.stats[0].x = geo.stats[1].x = UI_W / 9;

	// We need to compute text height.
	ui_clip(UI_FONT_STATS, &tw, &th, "lilou");
	h = geo.header.h;
	h -= th * 2;
	h /= 3;

	geo.stats[0].y = 5 + h;
	geo.stats[1].y = geo.stats[0].y + th + h;
}

static void
init_blocks(void)
{
	// Load all blocks.
	for (size_t i = 0; i < LEN(shapes); ++i)
		if (shapes[i].data)
			tex_load(&shapes[i].tex, shapes[i].data, shapes[i].datasz);
}

static void
init_board(void)
{
	struct tex *current;

	// Board geometry.
	geo.board.x = geo.header.x;
	geo.board.y = (UI_H * 2)  / 16;
	geo.board.w = geo.header.w;
	geo.board.h = (UI_H * 13) / 16;
	
	geo.board.cw = shapes[1].tex.w;
	geo.board.ch = shapes[1].tex.h;

	// Background board.
	tex_new(&boardbg, geo.board.w, geo.board.h);
	tex_alpha(&boardbg, 90);
	current = ui_target(&boardbg);

	// TODO: change this.
	ui_clear(UI_PALETTE_SHADOW);
	ui_target(current);
}

static int
can_move(int dx, int dy)
{
	struct shape shape = game.shape;

	// Try to put in the new place.
	shape.x += dx;
	shape.y += dy;

	return board_check(game.board, &shape);
}

static void
move(int dx, int dy)
{
	// First, remove the current shape to avoid collision with it.
	board_unset(game.board, &game.shape);

	if (can_move(dx, dy)) {
		sound_play(SOUND_MOVE);
		game.shape.x += dx;
		game.shape.y += dy;
	}

	board_set(game.board, &game.shape);
}

static void
drop(void)
{
	board_unset(game.board, &game.shape);

	while (can_move(0, 1))
		game.shape.y += 1;

	board_set(game.board, &game.shape);
	sound_play(SOUND_DROP);

	// Make the game spawn immediately.
	game.delay += game.fallrate;
}

static void
rotate(void)
{
	int o = game.shape.o;

	// As usual, unset before trying.
	board_unset(game.board, &game.shape);
	shape_rotate(&game.shape, 1);

	// Cancel orientation.
	if (!board_check(game.board, &game.shape))
		game.shape.o = o;

	board_set(game.board, &game.shape);
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

	tex_draw(&tex[1], geo.stats[row].x + 1, geo.stats[row].y + 1);
	tex_draw(&tex[0], geo.stats[row].x, geo.stats[row].y);
	tex_finish(&tex[0]);
	tex_finish(&tex[1]);
}

static inline int
level(void)
{
	if (game.lines >= 100)
		return 10;

	return (game.lines / 10) + 1;
}

static void
draw_stats(void)
{
	draw_stat(0, "level %d", level());
	draw_stat(1, "lines %d", game.lines);
}

static void
draw_borders(void)
{
	// -
	ui_draw_line(UI_PALETTE_BORDER,
	    geo.board.x - 1,
	    geo.board.y - 1,
	    geo.board.x + geo.board.w,
	    geo.board.y - 1);
	ui_draw_line(UI_PALETTE_BORDER,
	    geo.board.x - 1,
	    geo.board.y + geo.board.h,
	    geo.board.x + geo.board.w,
	    geo.board.y + geo.board.h);

	// |
	ui_draw_line(UI_PALETTE_BORDER,
	    geo.board.x - 1,
	    geo.board.y - 1,
	    geo.board.x - 1,
	    geo.board.y + geo.board.h);
	ui_draw_line(UI_PALETTE_BORDER,
	    geo.board.x + geo.board.w,
	    geo.board.y - 1,
	    geo.board.x + geo.board.w,
	    geo.board.y + geo.board.h);
}

static void
draw_board(void)
{
	int s;

	tex_draw(&boardbg, geo.board.x, geo.board.y);

	for (int r = 0; r < BOARD_H; ++r) {
		// If we're drawing full lines, we make it blink.
		if (anim.lines >> r & 0x1 && anim.blink)
			continue;

		for (int c = 0; c < BOARD_W; ++c) {
			if (!(s = game.board[r][c]))
				continue;

			tex_draw(&shapes[s].tex,
			    geo.board.x + (c * geo.board.cw),
			    geo.board.y + (r * geo.board.ch));
		}
	}
}

static void
draw_next(void)
{
	int wh;

	// Individual cell size.
	wh = geo.shape.s / 4;

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			if (!game.shape_next.def[0][r][c])
				continue;

			tex_scale(&shapes[game.shape_next.k].tex,
			    geo.shape.x + (c * wh),
			    geo.shape.y + (r * wh),
			    wh, wh);
		}
	}

}

static int
fall(void)
{
	int ret = 0;

	board_unset(game.board, &game.shape);

	if (can_move(0, 1)) {
		game.shape.y += 1;
		ret = 1;
	}

	board_set(game.board, &game.shape);

	return ret;
}

static void
spawn(void)
{
	// Move next shape to current and create a new one.
	game.shape = game.shape_next;
	game.shape.x = 3;
	game.shape.y = 0;

	// If we can't spawn, that's dead!
	if (!board_check(game.board, &game.shape))
		stris_switch(&state_dead);
	else {
		board_set(game.board, &game.shape);
		shape_select(&game.shape_next, game.rand);
	}
}

static void
init(void)
{
	init_pause();
	init_header();
	init_stats();
	init_blocks();
	init_board();
}

static void
nightmarize(void)
{
	for (int r = 10; r < BOARD_H; ++r)
		for (int c = 0; c < BOARD_W; ++c)
			if (NRAND(0, 1) == 0)
				game.board[r][c] = NRAND(1, SHAPE_RANDOM_STD);
}

static void
resume(void)
{
	// Depending on the mode.
	game.rand = state_play_mode >= STATE_PLAY_MODE_EXTENDED
		? SHAPE_RANDOM_EXT
		: SHAPE_RANDOM_STD;

	// Select new shapes and positionate the first one.
	shape_select(&game.shape, game.rand);
	shape_select(&game.shape_next, game.rand);

	game.shape.y = 0;
	game.shape.x = 3;
	game.fallrate = FALLRATE_INIT;

	// Apply that shape.
	board_clear(game.board);
	board_set(game.board, &game.shape);

	// In nightmare mode, we start with high level of blocks.
	if (state_play_mode == STATE_PLAY_MODE_NIGHTMARE)
		nightmarize();

	// Disable pause and clear scores.
	pause.enable = 0;
	game.lines = 0;

	// Cleanup animation.
	memset(&anim, 0, sizeof (anim));
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
	case KEY_UP:
		if (pause.enable)
			pause.enable = 0;
		else if (!anim.lines)
			rotate();
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
cleanup(void)
{
	int total, count = 0;

	anim.lines = 0;

	for (int r = 0; r < BOARD_H; ++r) {
		total = 0;

		for (int c = 0; c < BOARD_W; ++c)
			if (game.board[r][c])
				total++;

		if (total >= 10) {
			anim.lines |= 1 << r;
			count++;
		}
	}

	// No lines? Spawn immediately.
	if (anim.lines == 0)
		spawn();
	else {
		game.lines += count;
		sound_play(SOUND_CLEAN);
	}
}

static void
update_anim(int ticks)
{
	anim.spent += ticks;

	// Make the line blink.
	anim.blinkdelay += ticks;

	if (anim.blinkdelay >= ANIM_BLINK) {
		anim.blink = !anim.blink;
		anim.blinkdelay = 0;
	}

	if (anim.spent >= ANIM_DELAY) {
		anim.spent = anim.blink = anim.blinkdelay = 0;

		// Remove all lines.
		for (int i = 0; i < 20; ++i)
			if (anim.lines >> i & 0x1)
				board_pop(game.board, i);

		anim.lines = 0;
		spawn();
	}
}

static void
update_game(int ticks)
{
	game.delay += ticks;

	if (game.delay >= FALLRATE_INIT - (level() * FALLRATE_DECR)) {
		game.delay = 0;

		// At the bottom? Spawn a new shape otherwise just place the
		// new position.
		if (!fall())
			cleanup();
	}
}

static void
update(int ticks)
{
	if (pause.enable)
		return;

	ui_update_background(ramp[level() - 1], ticks);

	if (anim.lines)
		update_anim(ticks);
	else
		update_game(ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	ui_draw_background();

	if (pause.enable) {
		tex_draw(&pause.tex[1], geo.pause.x + 1, geo.pause.y + 1);
		tex_draw(&pause.tex[0], geo.pause.x, geo.pause.y);
	} else {
		draw_stats();
		draw_borders();
		draw_board();
		draw_next();
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

enum state_play_mode state_play_mode;

struct state state_play = {
	.init = init,
	.resume = resume,
	.update = update,
	.draw = draw,
	.onkey = onkey,
	.finish = finish
};
