/*
 * state-play.c -- game state
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

#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "key.h"
#include "list.h"
#include "score.h"
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
#include "img/block11.h"
#include "img/block12.h"

#define ANIM_DELAY      1000
#define ANIM_BLINK      100

#define FALLRATE_INIT   900
#define FALLRATE_DECR   74

#define MV_DELAY        230

#define SHAPE_TEX(d) { .data = d, sizeof (d) }

struct geo {
	int x;
	int y;
	int w;
	int h;
};

static struct {
	int lines;
	int pause;
	Board board;

	// Player movement (button on hold).
	int mv_ticks;
	int mv_stx;
	int mv_sty;

	// Current shape position.
	struct shape shape;

	// Based on https://tetris.fandom.com/wiki/Random_Generator
	struct shape shape_bag[SHAPE_RAND_NIGHTMARE];
	enum shape_rand shape_bag_rand;
	size_t shape_bag_iter;
} game;

static struct {
	// Board geometry and background texture.
	struct geo board_geo;
	struct tex board_bg;

	// Time for moving shapes.
	int ticks;

	// Next shape bounding box.
	struct geo next_shape_geo;

	// Label "Pause" in the middle of the screen.
	struct tex pause_text[2];
	struct geo pause_geo;

	// Animation.
	int anim_lines;
	int anim_ticks;
	int anim_blink;
	int anim_blinkticks;

	// Top stats (level, lines)
	char stat_level[16];
	char stat_lines[16];
	struct list_item stat_items[2];
	struct list stat_list;
} ui = {
	// Put default value to compute height.
	.stat_level = "level 1",
	.stat_lines = "lines 0",
	.stat_items = {
		{ .text = ui.stat_level },
		{ .text = ui.stat_lines }
	},
	.stat_list = {
		.font = UI_FONT_STATS,
		.halign = -1,
		.items = ui.stat_items,
		.itemsz = LEN(ui.stat_items)
	}
};

static struct {
	struct tex tex;
	const void *data;
	size_t datasz;
} shapes[] = {
	SHAPE_TEX(assets_img_block5),
	SHAPE_TEX(assets_img_block6),
	SHAPE_TEX(assets_img_block2),
	SHAPE_TEX(assets_img_block4),
	SHAPE_TEX(assets_img_block1),
	SHAPE_TEX(assets_img_block8),
	SHAPE_TEX(assets_img_block3),
	SHAPE_TEX(assets_img_block9),
	SHAPE_TEX(assets_img_block10),
	SHAPE_TEX(assets_img_block11),
	SHAPE_TEX(assets_img_block12),
	SHAPE_TEX(assets_img_block7),
};

static const uint32_t ramp[] = {
	0x33984bff,
	0x1e6f50ff,
	0x0098dcff,
	0x0069aaff,
	0xee8201ff,
	0xc1460fff,
	0x801224ff,
	0x590924ff,
	0x424c6eff,
	0x2a2f4eff
};

static void
init_shapes(void)
{
	// Load all blocks.
	for (size_t i = 0; i < LEN(shapes); ++i)
		if (shapes[i].data)
			tex_load(&shapes[i].tex, shapes[i].data, shapes[i].datasz);
}

static void
init_ui_pause(void)
{
	// Prepare pause label.
	ui_render(&ui.pause_text[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, "pause");
	ui_render(&ui.pause_text[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, "pause");
	ui.pause_geo.x = (UI_W - ui.pause_text[0].w) / 2;
	ui.pause_geo.y = (UI_H - ui.pause_text[0].h) / 2;
}

static void
init_ui_board(void)
{
	struct tex *current;

	// Board dimensions depend on the shape texture.
	ui.board_geo.w = shapes[1].tex.w * BOARD_W;
	ui.board_geo.h = shapes[1].tex.h * BOARD_H;

	// We center that board horizontally and apply the same padding
	// vertically from the bottom.
	ui.board_geo.x = (UI_W - ui.board_geo.w) / 2;
	ui.board_geo.y = UI_H - ui.board_geo.h - ui.board_geo.x;

	// Create a texture for the board background.
	tex_new(&ui.board_bg, ui.board_geo.w, ui.board_geo.h);
	tex_alpha(&ui.board_bg, 90);
	current = ui_target(&ui.board_bg);
	ui_clear(UI_PALETTE_SHADOW);
	ui_target(current);
}

static void
init_ui_next_shape(void)
{
	// The next shape box is located at the top right of the board and
	// takes the available height, width is equal to height.
	ui.next_shape_geo.w = ui.next_shape_geo.h = ui.board_geo.y;
	ui.next_shape_geo.x = ui.board_geo.x + ui.board_geo.w - ui.next_shape_geo.w;
}

static void
init_ui_stats(void)
{
	ui.stat_list.x = ui.board_geo.x;
	ui.stat_list.w = ui.board_geo.w;
	ui.stat_list.h = ui.board_geo.y;
	list_init(&ui.stat_list);
	list_reset(&ui.stat_list);
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

static inline void
start_move(int dx, int dy)
{
	game.mv_ticks = 0;

	if (dx)
		game.mv_stx = dx;
	if (dy)
		game.mv_sty = dy;

	move(dx, dy);
}

static inline void
stop_move(enum key key)
{
	//
	// This function may be called while we have pressed another direction.
	// For example in this order:
	//
	// 1. hold right arrow key (starts moving to right)
	// 2. hold left button (should start moving to left)
	// 3. release right button (cancels movement)
	//

	switch (key) {
	case KEY_RIGHT:
		if (game.mv_stx != -1)
			game.mv_stx = 0;
		break;
	case KEY_LEFT:
		if (game.mv_stx != 1)
			game.mv_stx = 0;
		break;
	case KEY_DOWN:
		game.mv_sty = 0;
		break;
	default:
		break;
	}
}

static void
drop(void)
{
	board_unset(game.board, &game.shape);

	while (can_move(0, 1))
		game.shape.y += 1;

	board_set(game.board, &game.shape);
	sound_play(SOUND_DROP);

	// Make sure the game will spawn a piece immediately.
	ui.ticks = 1000;
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
	else
		sound_play(SOUND_MOVE);

	board_set(game.board, &game.shape);
}

static inline int
level(void)
{
	if (game.lines >= 100)
		return 10;

	return (game.lines / 10) + 1;
}

static void
shuffle(void)
{
	// The array is large enough to store all pieces, we use the random
	// enum that contain the limit to use.
	switch (state_play_mode) {
	case STRIS_MODE_EXTENDED:
		game.shape_bag_rand = SHAPE_RAND_EXTENDED;
		break;
	case STRIS_MODE_NIGHTMARE:
		game.shape_bag_rand = SHAPE_RAND_NIGHTMARE;
		break;
	default:
		game.shape_bag_rand = SHAPE_RAND_STANDARD;
		break;
	}

	game.shape_bag_iter = 0;
	shape_shuffle(game.shape_bag, LEN(game.shape_bag), game.shape_bag_rand);
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
	game.shape = game.shape_bag[game.shape_bag_iter++];

	if (game.shape_bag_iter >= game.shape_bag_rand)
		shuffle();

	game.shape.x = 3;
	game.shape.y = 0;

	// If we can't spawn, that's dead!
	if (!board_check(game.board, &game.shape))
		stris_switch(&state_dead);
	else
		board_set(game.board, &game.shape);
}

static void
nightmarize(void)
{
	for (int r = 16; r < BOARD_H; ++r)
		for (int c = 0; c < BOARD_W; ++c)
			if (nrand(0, 1) == 0)
				game.board[r][c] = nrand(0, LEN(shapes) - 1);
}

static void
draw_stats(void)
{
	snprintf(ui.stat_level, sizeof (ui.stat_level), "level %d", level());
	snprintf(ui.stat_lines, sizeof (ui.stat_lines), "lines %d", game.lines);
	list_draw(&ui.stat_list);
}

static void
draw_borders(void)
{
	// -
	ui_draw_line(UI_PALETTE_BORDER,
	    ui.board_geo.x - 1,
	    ui.board_geo.y - 1,
	    ui.board_geo.x + ui.board_geo.w,
	    ui.board_geo.y - 1);
	ui_draw_line(UI_PALETTE_BORDER,
	    ui.board_geo.x - 1,
	    ui.board_geo.y + ui.board_geo.h,
	    ui.board_geo.x + ui.board_geo.w,
	    ui.board_geo.y + ui.board_geo.h);

	// |
	ui_draw_line(UI_PALETTE_BORDER,
	    ui.board_geo.x - 1,
	    ui.board_geo.y - 1,
	    ui.board_geo.x - 1,
	    ui.board_geo.y + ui.board_geo.h);
	ui_draw_line(UI_PALETTE_BORDER,
	    ui.board_geo.x + ui.board_geo.w,
	    ui.board_geo.y - 1,
	    ui.board_geo.x + ui.board_geo.w,
	    ui.board_geo.y + ui.board_geo.h);
}

static void
draw_board(void)
{
	int s;

	tex_draw(&ui.board_bg, ui.board_geo.x, ui.board_geo.y);

	for (int r = 0; r < BOARD_H; ++r) {
		// If we're drawing full lines, we make it blink.
		if (ui.anim_lines >> r & 0x1 && ui.anim_blink)
			continue;

		for (int c = 0; c < BOARD_W; ++c) {
			if (!(s = game.board[r][c]))
				continue;

			tex_draw(&shapes[s - 1].tex,
			    ui.board_geo.x + (c * shapes[1].tex.w),
			    ui.board_geo.y + (r * shapes[1].tex.h));
		}
	}
}

static void
draw_next_shape(void)
{
	int x, y, r, s, gap = 0, tmpmaxh, maxh = 0;
	const struct shape *next = &game.shape_bag[game.shape_bag_iter];

	// To align to the top right, we first need to know how many empty
	// cells are present at the right of the shape definition.
	for (int c = 3; c >= 0; --c) {
		for (r = 0; r < 4; ++r)
			if (next->def[0][r][c])
				goto out;
		if (r == 4)
			gap++;
	}

out:
	// Now we compute the max height that this shape requires.
	for (int c = 0; c < 4; ++c) {
		tmpmaxh = 0;

		for (r = 0; r < 4; ++r)
			if (next->def[0][r][c])
				tmpmaxh ++;

		if (tmpmaxh > maxh)
			maxh = tmpmaxh;
	}

	// We compute the height available between the top and the game board
	// and remove some padding to avoid being snapped with the board.
	s = shapes[1].tex.w / 2;
	x = (ui.board_geo.x + ui.board_geo.w) - (s * (4 - gap));
	y = ((ui.board_geo.y - 10) / 2) - ((s * maxh) / 2);

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			if (!next->def[0][r][c])
				continue;

			tex_scale(&shapes[next->k].tex,
			    x + (c * s), y + (r * s), s, s);
		}
	}
}

static void
cleanup(void)
{
	int total, count = 0;

	ui.anim_lines = 0;

	for (int r = 0; r < BOARD_H; ++r) {
		total = 0;

		for (int c = 0; c < BOARD_W; ++c)
			if (game.board[r][c])
				total++;

		if (total >= 10) {
			ui.anim_lines |= 1 << r;
			count++;
		}
	}

	// No lines? Spawn immediately.
	if (ui.anim_lines == 0)
		spawn();
	else {
		game.lines += count;
		sound_play(SOUND_CLEAN);
	}
}

static void
update_anim(int ticks)
{
	// Make the line blink.
	ui.anim_ticks += ticks;
	ui.anim_blinkticks += ticks;

	if (ui.anim_blinkticks >= ANIM_BLINK) {
		ui.anim_blink = !ui.anim_blink;
		ui.anim_blinkticks = 0;
	}

	if (ui.anim_ticks >= ANIM_DELAY) {
		ui.anim_ticks = ui.anim_blink = ui.anim_blinkticks = 0;

		// Remove all lines.
		for (int i = 0; i < 20; ++i)
			if (ui.anim_lines >> i & 0x1)
				board_pop(game.board, i);

		ui.anim_lines = 0;
		spawn();
	}
}

static void
update_game(int ticks)
{
	ui.ticks += ticks;

	if (ui.ticks >= FALLRATE_INIT - (level() * FALLRATE_DECR)) {
		ui.ticks = 0;

		// At the bottom? Spawn a new shape otherwise just place the
		// new position.
		if (!fall())
			cleanup();
	}
}

static void
init(void)
{
	init_shapes();
	init_ui_pause();
	init_ui_board();
	init_ui_next_shape();
	init_ui_stats();
}

static void
resume(void)
{
	// Select new shapes and positionate the first one.
	shuffle();

	game.shape = game.shape_bag[game.shape_bag_iter++];
	game.shape.y = 0;
	game.shape.x = 3;

	// Apply that shape.
	board_clear(game.board);
	board_set(game.board, &game.shape);

	// In nightmare mode, we start with high level of blocks.
	if (state_play_mode == STRIS_MODE_NIGHTMARE)
		nightmarize();

	// Disable pause and clear scores.
	game.pause = game.lines = 0;
}

static void
onkey(enum key key, int state)
{
	if (!state)
		stop_move(key);

	switch (key) {
	case KEY_CANCEL:
		if (!state)
			break;

		// Already in pause, escape to main menu then.
		if (game.pause)
			stris_switch(&state_menu);
		else
			game.pause = 1;
		break;
	case KEY_SELECT:
	case KEY_UP:
		if (game.pause)
			game.pause = 0;
		else if (!ui.anim_lines && state)
			rotate();
		break;
	case KEY_RIGHT:
		if (state && game.mv_stx != 1)
			start_move(1, 0);
		break;
	case KEY_DOWN:
		if (state && game.mv_sty != 1)
			start_move(0, 1);
		break;
	case KEY_LEFT:
		if (state && game.mv_stx != -1)
			start_move(-1, 0);
		break;
	case KEY_DROP:
		if (state)
			drop();
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
	if (game.pause)
		return;

	game.mv_ticks += ticks;

	if (game.mv_ticks >= MV_DELAY) {
		game.mv_ticks = 0;

		if (game.mv_stx || game.mv_sty)
			move(game.mv_stx, game.mv_sty);
	}

	ui_update_background(ramp[level() - 1], ticks);

	if (ui.anim_lines)
		update_anim(ticks);
	else
		update_game(ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	ui_draw_background();

	if (game.pause) {
		tex_draw(&ui.pause_text[1], ui.pause_geo.x + 1, ui.pause_geo.y + 1);
		tex_draw(&ui.pause_text[0], ui.pause_geo.x,     ui.pause_geo.y);
	} else {
		draw_stats();
		draw_borders();
		draw_board();
		draw_next_shape();
	}

	ui_present();
}

static void
suspend(void)
{
	struct score_list list;
	struct score sc;
	const char *path;

	snprintf(sc.who, sizeof (sc.who), "%s", username());
	sc.lines = game.lines;

	path = score_path(state_play_mode);
	score_read(&list, path);
	score_add(&list, &sc);
	score_write(&list, path);
}

static void
finish(void)
{
	tex_finish(&ui.board_bg);
	tex_finish(&ui.pause_text[0]);
	tex_finish(&ui.pause_text[1]);

	for (size_t i = 0; i < LEN(shapes); ++i)
		tex_finish(&shapes[i].tex);
}

enum stris_mode state_play_mode;

struct state state_play = {
	.init = init,
	.resume = resume,
	.update = update,
	.draw = draw,
	.onkey = onkey,
	.suspend = suspend,
	.finish = finish
};
