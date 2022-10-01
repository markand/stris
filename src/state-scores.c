/*
 * state-scores.c -- show high scores
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

#include <assert.h>

#include "key.h"
#include "list.h"
#include "score.h"
#include "state-menu.h"
#include "state-scores.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

#define HEADER_HEIGHT (2 * UI_H / 16)

//
// View is as following:
//
// +----------------+ -^-
// | <    Mode    > |  |          2 * (UI_H / 16)
// +----------------+ -v-
// | Jean       123 |
// | Francis    100 |
// | Liloutre    80 |
// |                |
// |                |
// |                |
// +----------------+
//
struct view {
	char names[SCORE_LIST_MAX][SCORE_NAME_MAX];
	char lines[SCORE_LIST_MAX][16];
	struct list_item item_names[SCORE_LIST_MAX];
	struct list_item item_lines[SCORE_LIST_MAX];
	struct list list_names;
	struct list list_lines;
};

//
// 0: standard
// 1: extended
// 2: nightmare
//
static struct view views[3];
static size_t selected;
static struct tex arrow_left[2];
static struct tex arrow_right[2];

static void
init(void)
{
	// Common properties for both lists.
	for (size_t m = 0; m < 3; ++m) {
		views[m].list_names.font   = views[m].list_lines.font   = UI_FONT_MENU_SMALL;
		views[m].list_names.valign = views[m].list_lines.valign = -1;
		views[m].list_names.p      = views[m].list_lines.p      = 10;
		views[m].list_names.y      = views[m].list_lines.y      = HEADER_HEIGHT;
		views[m].list_names.w      = views[m].list_lines.w      = UI_W;
		views[m].list_names.h      = views[m].list_lines.h      = UI_H - views[m].list_names.y;

		views[m].list_names.items  = views[m].item_names;
		views[m].list_lines.items  = views[m].item_lines;

		// Link list items to the appropriate strings.
		for (size_t i = 0; i < SCORE_LIST_MAX; ++i) {
			views[m].item_names[i].text = views[m].names[i];
			views[m].item_lines[i].text = views[m].lines[i];
		}
	}

	// Names are left aligned.
	for (size_t m = 0; m < 3; ++m)
		views[m].list_names.halign = -1;

	// Lines are right aligned.
	for (size_t m = 0; m < 3; ++m)
		views[m].list_lines.halign = 1;

	// Create left/right arrows.
	ui_render(&arrow_left[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, "<");
	ui_render(&arrow_left[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, "<");
	ui_render(&arrow_right[0], UI_FONT_MENU_SMALL, UI_PALETTE_FG, ">");
	ui_render(&arrow_right[1], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, ">");

}

static void
populate(struct view *v, const char *path)
{
	struct score_list list;

	// Open score file before filling up the list.
	score_read(&list, path);

	// Populate left (names) and right (lines) lists with that score file.
	for (size_t i = 0; i < list.scoresz; ++i) {
		snprintf(v->names[i], sizeof (v->names[i]), "%s", list.scores[i].who);
		snprintf(v->lines[i], sizeof (v->lines[i]), "%d", list.scores[i].lines);
	}

	v->list_names.itemsz = v->list_lines.itemsz = list.scoresz;

	list_init(&v->list_names);
	list_reset(&v->list_names);

	list_init(&v->list_lines);
	list_reset(&v->list_lines);
}

static void
draw_header(void)
{
	static const char * const names[] = { "standard", "extended", "nightmare" };
	struct tex tex[2];
	int x, y;

	ui_render(&tex[0], UI_FONT_MENU, UI_PALETTE_FG, names[selected]);
	ui_render(&tex[1], UI_FONT_MENU, UI_PALETTE_SHADOW, names[selected]);

	x = (UI_W - tex[0].w) / 2;
	y = (HEADER_HEIGHT / 2) - (tex[0].h / 2);

	tex_draw(&tex[1], x + 1, y + 1);
	tex_draw(&tex[0], x, y);
	tex_finish(&tex[0]);
	tex_finish(&tex[1]);
}

static void
draw_arrows(void)
{
	int x, y;

	// <
	x = 10;
	y = (HEADER_HEIGHT / 2) - (arrow_left[0].h / 2);
	tex_draw(&arrow_left[1], x + 1 , y + 1);
	tex_draw(&arrow_left[0], x, y);

	// >
	x = UI_W - arrow_right[0].w - 10;
	tex_draw(&arrow_right[1], x + 1 , y + 1);
	tex_draw(&arrow_right[0], x, y);

}

static inline void
draw_lists(void)
{
	list_draw(&views[selected].list_names);
	list_draw(&views[selected].list_lines);
}


static void
resume(void)
{
	for (size_t m = 0; m < 3; ++m)
		populate(&views[m], score_path(m));

	selected = 0;
}

static void
onkey(enum key key, int state)
{
	if (!state)
		return;

	switch (key) {
	case KEY_LEFT:
		if (selected == 0)
			selected = 2;
		else
			selected --;
		break;
	case KEY_SELECT:
	case KEY_RIGHT:
		if (selected == 2)
			selected = 0;
		else
			selected ++;
		break;
	case KEY_CANCEL:
		stris_switch(&state_menu);
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
	ui_update_background(UI_PALETTE_MENU_BG, ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	ui_draw_background();
	draw_header();
	draw_arrows();
	draw_lists();
	ui_present();
}

static void
finish(void)
{
	tex_finish(&arrow_left[0]);
	tex_finish(&arrow_left[1]);
	tex_finish(&arrow_right[0]);
	tex_finish(&arrow_right[1]);
}

struct state state_scores = {
	.init = init,
	.resume = resume,
	.onkey = onkey,
	.update = update,
	.draw = draw,
	.finish = finish
};
