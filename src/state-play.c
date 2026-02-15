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

#include <assert.h>
#include <stdio.h>

#include <SDL3/SDL.h>

#include "board.h"
#include "coroutine.h"
#include "node.h"
#include "score.h"
#include "shape.h"
#include "sound.h"
#include "state-menu.h"
#include "stris.h"
#include "texture.h"
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

#define SCENE(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct scene, Field))

#define PLAY_INIT_SHAPE(Scene, Index, Image)                                    \
do {                                                                            \
        texture_load(&(Scene)->shapes[(Index)], (Image), sizeof ((Image)));     \
} while (0)

enum fallrate {
	FALLRATE_INIT = 900,
	FALLRATE_DECR = 74
};

enum state {
	RUNNING,
	PAUSED,
	ANIMATING,
	DEAD,
	TERMINATED
};

struct label {
	struct texture texture;
	struct node node;
};

struct scene {
	enum state state;
	enum mode mode;

	/* current stats */
	unsigned int level;
	unsigned int lines;

	/* top level stats */
	struct label lbl_level;
	struct label lbl_lines;

	/* game board background */
	struct node bg;

	/* game board foreground */
	struct node fg;

	/* next shape texture. */
	struct node next;

	/* game board and current shape moving */
	Board board;
	struct shape shape;

	/*
	 * Bag of shapes that are incoming.
	 *
	 * They are not generated equal but on a more useful list to avoid
	 * getting too many consecutive annoying shapes.
	 *
	 * See https://tetris.fandom.com/wiki/Random_Generator
	 */
	struct shape shape_bag[SHAPE_RAND_NIGHTMARE];
	enum shape_rand shape_bag_rand;
	size_t shape_bag_iter;

	/* pause overall overlay */
	struct node pause;

	/* logic coroutine */
	struct coroutine logic;

	/* input handler coroutine */
	struct coroutine input;

	/* loaded texture for every shape */
	struct texture shapes[SHAPE_RAND_MAX];
};

static void
play_init_bg(struct scene *scene)
{
	struct texture texture;
	unsigned int w, h;

	/*
	 * Background dimensions is number of blocks + 2 pixels in each
	 * direction to add a small border.
	 */
	w = (scene->shapes->w * BOARD_W) + 2;
	h = (scene->shapes->h * BOARD_H) + 2;

	texture_init(&texture,  w, h);

	UI_BEGIN(&texture);
	texture_alpha(&texture, 90);
	ui_clear(UI_PALETTE_SHADOW);

	/* - */
	ui_draw_line(UI_PALETTE_BORDER, 0, 0, w, 0);
	ui_draw_line(UI_PALETTE_BORDER, 0, h - 1, w, h - 1);

	/* | */
	ui_draw_line(UI_PALETTE_BORDER, 0, 0, 0, h);
	ui_draw_line(UI_PALETTE_BORDER, w - 1, 0, w - 1, h);

	scene->bg.x = (UI_W - texture.w) / 2;
	scene->bg.y = (UI_H - texture.h) - scene->bg.x;
	node_wrap(&scene->bg, &texture);
	UI_END();
}

static void
play_init_fg(struct scene *scene)
{
	struct texture texture;

	/*
	 * Foreground is off by 1 pixel because background contains a 1 pixel
	 * border in each direction.
	 */
	texture_init(&texture, scene->bg.texture->w - 2, scene->bg.texture->h - 2);
	scene->fg.x = scene->bg.x + 1;
	scene->fg.y = scene->bg.y + 1;
	node_wrap(&scene->fg, &texture);
}

static void
play_update_stat(struct scene *scene)
{
	ui_printf_shadowed(&scene->lbl_level.texture, UI_FONT_STATS, UI_PALETTE_FG, "level %u", scene->level);
	ui_printf_shadowed(&scene->lbl_lines.texture, UI_FONT_STATS, UI_PALETTE_FG, "lines %u", scene->lines);
}

static void
play_update_next_shape(struct scene *scene)
{
	const struct shape *next = &scene->shape_bag[scene->shape_bag_iter];
	unsigned int rows, columns, w, h;
	int x, y;

	/*
	 * Get how many rows and columns are required to draw this shape so we
	 * vertically center and horizontally right justified.
	 */
	rows = shape_max_rows(next);
	columns = shape_max_columns(next);

	assert(rows);
	assert(columns);

	/* Adjust to texture size. */
	w = columns * (scene->shapes->w / 2);
	h = rows * (scene->shapes->h / 2);

	/* Render the shape on the texture with half width/height. */
	UI_BEGIN(scene->next.texture);
	ui_clear(0x00000000);

	y = (scene->next.texture->h / 2) - (h / 2);

	for (int r = 0; r < 4; ++r) {
		x = (scene->next.texture->w) - (w);

		if (columns != 4)
			x -= scene->shapes->w / 2;

		for (int c = 0; c < 4; ++c) {
			if (next->def[0][r][c])
				texture_scale(&scene->shapes[next->k],
				    x, y,
				    scene->shapes->w / 2,
				    scene->shapes->h / 2);

			x += scene->shapes->w / 2;
		}

		y += scene->shapes->h / 2;
	}

	UI_END();
}

static void
play_update_board(struct scene *scene, unsigned int mask, unsigned int alpha)
{
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

	int s;

	UI_BEGIN(scene->fg.texture);
	ui_clear(0x00000000);

	for (int r = 0; r < BOARD_H; ++r) {
		for (int c = 0; c < BOARD_W; ++c) {
			if (!(s = scene->board[r][c]))
				continue;

			if (mask >> r & 0x1)
				texture_alpha(&scene->shapes[s - 1], alpha);
			else
				texture_alpha(&scene->shapes[s - 1], 255);

			ui_background_set(ramp[scene->level - 1]);
			texture_render(&scene->shapes[s - 1],
			    (c * scene->shapes[1].w),
			    (r * scene->shapes[1].h));
		}
	}

	UI_END();
}

/*
 * Level and lines are shown above the background and vertically centered.
 */
static void
play_init_stat(struct scene *scene)
{
	unsigned int vheight;

	/* Start with default values */
	play_update_stat(scene);

	/*
	 * This is the remaining space between the top of the window and the
	 * beginning of the game view.
	 */
	vheight = scene->bg.y;

	/* level <n> */
	scene->lbl_level.node.x = scene->bg.x;
	scene->lbl_level.node.y = vheight / 4 - scene->lbl_level.texture.h / 2;
	scene->lbl_level.node.texture = &scene->lbl_level.texture;
	node_init(&scene->lbl_level.node);

	/* lines <n> */
	scene->lbl_lines.node.x = scene->bg.x;
	scene->lbl_lines.node.y = vheight - (vheight / 3) - (scene->lbl_lines.texture.h / 2);
	scene->lbl_lines.node.texture = &scene->lbl_lines.texture;
	node_init(&scene->lbl_lines.node);
}

/*
 * Initialize the texture that is used to render the next shape.
 *
 * It is located just above the game panel view and right justified.
 */
static void
play_init_next(struct scene *scene)
{
	struct texture texture;

	texture_init(&texture, scene->bg.y, scene->bg.y);
	scene->next.x = scene->bg.x + scene->bg.texture->w - scene->bg.y;
	node_wrap(&scene->next, &texture);

	play_update_next_shape(scene);
}

static void
play_init_shapes(struct scene *scene)
{
	PLAY_INIT_SHAPE(scene,  0, assets_img_block5);
	PLAY_INIT_SHAPE(scene,  1, assets_img_block6);
	PLAY_INIT_SHAPE(scene,  2, assets_img_block2);
	PLAY_INIT_SHAPE(scene,  3, assets_img_block4);
	PLAY_INIT_SHAPE(scene,  4, assets_img_block1);
	PLAY_INIT_SHAPE(scene,  5, assets_img_block8);
	PLAY_INIT_SHAPE(scene,  6, assets_img_block3);
	PLAY_INIT_SHAPE(scene,  7, assets_img_block9);
	PLAY_INIT_SHAPE(scene,  8, assets_img_block10);
	PLAY_INIT_SHAPE(scene,  9, assets_img_block11);
	PLAY_INIT_SHAPE(scene, 10, assets_img_block12);
	PLAY_INIT_SHAPE(scene, 11, assets_img_block7);
}

static void
play_init_pause(struct scene *scene)
{
	struct texture texture, text;

	ui_printf(&text, UI_FONT_MENU, UI_PALETTE_FG, "pause");

	texture_init(&texture, UI_W, UI_H);
	UI_BEGIN(&texture);
	ui_clear(UI_PALETTE_PAUSE_BG);
	texture_render(&text, (UI_W / 2) - (text.w / 2), (UI_H / 2) - (text.h / 2));
	texture_finish(&text);
	UI_END();

	scene->pause.hide = 1;
	node_wrap(&scene->pause, &texture);
}

static int
play_can_move(struct scene *scene, int dx, int dy)
{
	struct shape shape = scene->shape;

	/* Try to put in the new place. */
	shape.x += dx;
	shape.y += dy;

	return board_check(scene->board, &shape);
}

static void
play_move(struct scene *scene, enum key keys)
{
	int dx = 0, dy = 0, moved;

	if (keys & KEY_LEFT)
		dx = -1;
	else if (keys & KEY_RIGHT)
		dx = +1;

	if (keys & KEY_DOWN)
		dy += 1;

	if (dx || dy) {
		board_unset(scene->board, &scene->shape);

		if ((moved = play_can_move(scene, dx, dy))) {
			scene->shape.x += dx;
			scene->shape.y += dy;
		}

		board_set(scene->board, &scene->shape);

		/*
		 * Redraw board immediately. If the direction is going bottom,
		 * we rearm the fall rate delay to avoid spawning after the
		 * initial wait time already passed.
		 */
		play_update_board(scene, 0, 0);

		if (dy && moved)
			coroutine_rearm(&scene->logic);
	}
}

static void
play_rotate(struct scene *scene, enum key keys)
{
	int o;

	if (!(keys & KEY_UP))
		return;

	o = scene->shape.o;

	/* As usual, unset before trying. */
	board_unset(scene->board, &scene->shape);
	shape_rotate(&scene->shape, 1);

	/* Cancel orientation. */
	if (!board_check(scene->board, &scene->shape))
		scene->shape.o = o;
	else
		sound_play(SOUND_MOVE);

	board_set(scene->board, &scene->shape);
	play_update_board(scene, 0, 0);
}

static void
play_drop(struct scene *scene, enum key keys)
{
	if (!(keys & KEY_DROP))
		return;

	board_unset(scene->board, &scene->shape);

	while (play_can_move(scene, 0, 1))
		scene->shape.y += 1;

	board_set(scene->board, &scene->shape);
	sound_play(SOUND_DROP);

	/* Make sure the game will spawn a piece immediately. */
	coroutine_cancel(&scene->logic);
}

static void
play_input_entry(struct coroutine *self)
{
	struct scene *scene;
	enum key keys;

	scene = SCENE(self, input);

	while (scene->state != TERMINATED) {
		keys = stris_pressed();

		switch (scene->state) {
		case RUNNING:
			if (keys & KEY_CANCEL) {
				scene->state = PAUSED;
				scene->pause.hide = 0;
				scene->logic.pause = 1;
			} else {
				play_move(scene, keys);
				play_rotate(scene, keys);
				play_drop(scene, keys);
			}
			break;
		case PAUSED:
			if (keys & KEY_CANCEL) {
				scene->state = TERMINATED;
				scene->logic.pause = 0;
				coroutine_cancel(&scene->logic);
			} else if (keys & KEY_SELECT) {
				scene->state = RUNNING;
				scene->pause.hide = 1;
				scene->logic.pause = 0;
			}
			break;
		case DEAD:
			if (keys & (KEY_CANCEL | KEY_SELECT))
				scene->state = TERMINATED;
			break;
		default:
			break;
		}
	}
}

static void
play_shuffle(struct scene *scene)
{
	scene->shape_bag_iter = 0;
	shape_shuffle(scene->shape_bag, LEN(scene->shape_bag), scene->shape_bag_rand);
}

static void
play_spawn(struct scene *scene)
{
	/* Move next shape to current and create a new one. */
	scene->shape = scene->shape_bag[scene->shape_bag_iter++];

	if (scene->shape_bag_iter >= scene->shape_bag_rand)
		play_shuffle(scene);

	scene->shape.x = 3;
	scene->shape.y = 0;

	/* If we can't spawn, that's dead! */
	if (!board_check(scene->board, &scene->shape)) {
		scene->state = DEAD;

		/*
		 * Animate a line per line full board from bottom to top
		 * gradually
		 */
		for (int r = BOARD_H; r >= 0; --r) {
			sound_play(SOUND_TICK);

			for (int c = 0; c < BOARD_W; ++c)
				scene->board[r][c] = 11;

			play_update_board(scene, 0, 0);
			coroutine_sleep(40);
		}

		coroutine_sleep(500);
	} else {
		board_set(scene->board, &scene->shape);
		play_update_next_shape(scene);
	}
}

static int
play_fall(struct scene *scene)
{
	int ret = 0;

	board_unset(scene->board, &scene->shape);

	if (play_can_move(scene, 0, 1)) {
		scene->shape.y += 1;
		ret = 1;
	}

	board_set(scene->board, &scene->shape);

	return ret;
}

static void
play_cleanup(struct scene *scene)
{
	static const unsigned int alpharamp[] = {
		255, 230, 205, 180, 155, 130, 105,  80,
		 55,  30,   5,  30,  55,  80, 105, 130,
		155, 180, 205, 230, 255, 205, 155, 105,
		 55,   5,  55, 105, 155, 205, 255,   0,
		255,   0, 255,   0, 255,   0,   0, 255
	};

	unsigned int columns, count = 0;
	unsigned int lines = 0;

	/* This is a bitmask of lines full. */
	lines = 0;

	for (int r = 0; r < BOARD_H; ++r) {
		columns = 0;

		for (int c = 0; c < BOARD_W; ++c)
			if (scene->board[r][c])
				columns++;

		if (columns >= BOARD_W) {
			lines |= 1 << r;
			count++;
		}
	}

	if (count) {
		scene->lines += count;

		/* Recompute level but cap to 10. */
		if (scene->lines >= 100)
			scene->level = 10;

		scene->level = (scene->lines / 10) + 1;

		play_update_stat(scene);
		sound_play(SOUND_CLEAN);

		/* Animate pending lines. */
		scene->state = ANIMATING;

		for (size_t i = 0; i < LEN(alpharamp); ++i) {
			play_update_board(scene, lines, alpharamp[i]);
			coroutine_sleep(20);
		}

		for (int i = 0; i < BOARD_H; ++i)
			if ((lines >> i) & 1)
				board_pop(scene->board, i);

		scene->state = RUNNING;
	}

	play_spawn(scene);
}

static void
play_logic_entry(struct coroutine *self)
{
	struct scene *scene;

	scene = SCENE(self, logic);

	switch (scene->mode) {
	case MODE_EXTENDED:
		scene->shape_bag_rand = SHAPE_RAND_EXTENDED;
		break;
	case MODE_NIGHTMARE:
		scene->shape_bag_rand = SHAPE_RAND_NIGHTMARE;

		for (int r = 16; r < BOARD_H; ++r)
			for (int c = 0; c < BOARD_W; ++c)
				if (nrand(0, 1) == 0)
					scene->board[r][c] = nrand(0, LEN(scene->shapes) - 1);
		break;
	default:
		scene->shape_bag_rand = SHAPE_RAND_STANDARD;
		break;
	}

	play_shuffle(scene);

	play_init_shapes(scene);
	play_init_bg(scene);
	play_init_fg(scene);
	play_init_stat(scene);
	play_init_next(scene);
	play_init_pause(scene);

	play_spawn(scene);

	while (scene->state == RUNNING) {
		play_update_board(scene, 0, 0);

		/* Wait for fall. */
		coroutine_sleep(FALLRATE_INIT - (scene->level * FALLRATE_DECR));

		if (!play_fall(scene))
			play_cleanup(scene);
	}
}

static void
play_logic_terminate(struct coroutine *self)
{
	struct score_list list = {};
	struct score score = {};
	struct scene *scene;

	scene = SCENE(self, logic);

	node_finish(&scene->pause);
	node_finish(&scene->bg);
	node_finish(&scene->fg);

	node_finish(&scene->lbl_lines.node);
	texture_finish(&scene->lbl_lines.texture);

	node_finish(&scene->lbl_level.node);
	texture_finish(&scene->lbl_level.texture);

	for (size_t i = 0; i < LEN(scene->shapes); ++i)
		texture_finish(&scene->shapes[i]);

	node_finish(&scene->next);

	/* Save scores. */
	SDL_strlcpy(score.who, username(), sizeof (score.who));
	score.lines = scene->lines;

	if (scene->state == DEAD) {
		score_read(&list, score_path(scene->mode));
		score_add(&list, &score);
		score_write(&list, score_path(scene->mode));
	}

	/* Back to the menu. */
	ui_background_set(UI_PALETTE_MENU_BG);
	menu_run();
}

void
play_run(enum mode mode)
{
	struct scene *scene;

	scene = alloc(1, sizeof (*scene));
	scene->mode = mode;
	scene->level = 1;

	/* logic handler */
	scene->logic.entry = play_logic_entry;
	scene->logic.terminate = play_logic_terminate;
	coroutine_init(&scene->logic);

	/* input handler */
	scene->input.entry = play_input_entry;
	coroutine_init(&scene->input);
}
