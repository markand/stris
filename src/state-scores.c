/*
 * state-scores.c -- show high scores
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

#include "coroutine.h"
#include "list.h"
#include "node.h"
#include "score.h"
#include "state-menu.h"
#include "stris.h"
#include "texture.h"
#include "ui.h"
#include "util.h"

#define HEADER_HEIGHT (2 * UI_H / 16)

#define SCORES(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct scores, Field))

/*
 * View is as following:
 *
 * +----------------+ -^-
 * | <    Mode    > |  |          2 * (UI_H / 16)
 * +----------------+ -v-
 * | Jean       123 |
 * | Francis    100 |
 * | Liloutre    80 |
 * |                |
 * |                |
 * |                |
 * +----------------+
 */

struct scores {
	/* array of score */
	char names[SCORE_LIST_MAX][SCORE_NAME_MAX + 1];
	char lines[SCORE_LIST_MAX][16];

	/* names and their scores */
	struct list_item item_names[SCORE_LIST_MAX];
	struct list_item item_lines[SCORE_LIST_MAX];
	struct list list_names;
	struct list list_lines;

	/* <> selector */
	struct node arrow_left;
	struct node arrow_right;

	/* mode name on top */
	struct node mode[MODE_LAST];

	/* main entrypoint */
	struct coroutine coroutine;
};

static const char *mode_names[] = {
	[MODE_STANDARD]  = "standard",
	[MODE_EXTENDED]  = "extended",
	[MODE_NIGHTMARE] = "nightmare"
};

static void
scores_load(struct scores *scores, const char *path)
{
	struct score_list list;

	/* Cleanup and destroy before refilling. */
	list_finish(&scores->list_names);
	list_finish(&scores->list_lines);

	score_read(&list, path);

	/* Populate left (names) and right (lines) lists with that score file. */
	for (size_t i = 0; i < list.scoresz; ++i) {
		snprintf(scores->names[i], sizeof (scores->names[i]), "%s", list.scores[i].who);
		snprintf(scores->lines[i], sizeof (scores->lines[i]), "%d", list.scores[i].lines);
	}

	scores->list_names.font   = scores->list_lines.font   = UI_FONT_MENU_SMALL;
	scores->list_names.valign = scores->list_lines.valign = -1;
	scores->list_names.p      = scores->list_lines.p      = 10;
	scores->list_names.y      = scores->list_lines.y      = HEADER_HEIGHT;
	scores->list_names.w      = scores->list_lines.w      = UI_W;
	scores->list_names.h      = scores->list_lines.h      = UI_H - scores->list_names.y;
	scores->list_names.items  = scores->item_names;
	scores->list_lines.items  = scores->item_lines;
	scores->list_names.itemsz = scores->list_lines.itemsz = list.scoresz;

	/* Not selectable. */
	scores->list_lines.readonly = 1;
	scores->list_names.readonly = 1;

	/* Link list items to the appropriate strings. */
	for (size_t i = 0; i < SCORE_LIST_MAX; ++i) {
		scores->item_names[i].text = scores->names[i];
		scores->item_lines[i].text = scores->lines[i];
	}

	/* Names are left aligned. */
	for (size_t m = 0; m < 3; ++m)
		scores->list_names.halign = -1;

	/* Lines are right aligned. */
	for (size_t m = 0; m < 3; ++m)
		scores->list_lines.halign = 1;

	list_init(&scores->list_names);
	list_init(&scores->list_lines);
}

static void
scores_entry(struct coroutine *self)
{
	struct texture texture = {};
	struct scores *scores;
	size_t selected = 0;
	enum key keys;

	scores = SCORES(self, coroutine);

	/* < */
	ui_printf_shadowed(&texture, UI_FONT_MENU_SMALL, UI_PALETTE_FG, "<");
	scores->arrow_left.x = 10;
	scores->arrow_left.y = (HEADER_HEIGHT / 2) - (texture.h / 2);
	node_wrap(&scores->arrow_left, &texture);

	/* > */
	ui_printf_shadowed(&texture, UI_FONT_MENU_SMALL, UI_PALETTE_FG, ">");
	scores->arrow_right.x = UI_W - texture.w - 10;
	scores->arrow_right.y = (HEADER_HEIGHT / 2) - (texture.h / 2);
	node_wrap(&scores->arrow_right, &texture);

	/* The three titles, we will hide/show depending on selection. */
	for (size_t i = 0; i < MODE_LAST; ++i) {
		ui_printf_shadowed(&texture, UI_FONT_MENU_SMALL, UI_PALETTE_FG, "%s", mode_names[i]);
		scores->mode[i].y = (HEADER_HEIGHT / 2) - (texture.h / 2);
		scores->mode[i].x = (UI_W / 2) - (texture.w / 2);
		scores->mode[i].hide = 1;
		node_wrap(&scores->mode[i], &texture);
	}

	scores->mode->hide = 0;

	/* Common properties for both lists. */
	do {
		/* Reload scores depending on the mode. */
		scores_load(scores, score_path(selected));

		/* Wait for something. */
		keys = stris_pressed();

		if (keys & KEY_LEFT) {
			if (selected == 0)
				selected = 2;
			else
				selected --;
		} else if (keys & KEY_RIGHT) {
			if (selected == 2)
				selected = 0;
			else
				selected ++;
		}

		/* Hide every title but the currently selected. */
		for (size_t i = 0; i < MODE_LAST; ++i)
			scores->mode[i].hide = 1;

		scores->mode[selected].hide = 0;
	} while (!(keys & KEY_CANCEL));

	menu_run();
}

static void
scores_terminate(struct coroutine *self)
{
	struct scores *scores = SCORES(self, coroutine);

	for (size_t i = 0; i < MODE_LAST; ++i)
		node_finish(&scores->mode[i]);

	node_finish(&scores->arrow_right);
	node_finish(&scores->arrow_left);

	list_finish(&scores->list_lines);
	list_finish(&scores->list_names);
}

void
scores_run(void)
{
	struct scores *scores;

	scores = alloc(1, sizeof (*scores));
	scores->coroutine.entry = scores_entry;
	scores->coroutine.terminate = scores_terminate;
	coroutine_init(&scores->coroutine);
}
