/*
 * state-menu.c -- main menu
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

#include <stdio.h>

#include "coroutine.h"
#include "list.h"
#include "node.h"
#include "state-mode.h"
#include "state-scores.h"
#include "state-settings.h"
#include "stris.h"
#include "texture.h"
#include "ui.h"
#include "util.h"

#define MENU(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct menu, Field))

#define MENU_TITLE_X(Menu) ((UI_W - (Menu)->title[0].texture->w) / 2)

#define MENU_TITLE_Y(Menu) ((Menu)->title[0].texture->h / 2)

/*
 * View is as following:
 *
 * +----------------+ -^-
 * |     STris      |  |          4 * (UI_H / 16)
 * +----------------+ -v- -^-
 * |                |      |
 * |      Play      |      |
 * |     Scores     |      |     12 * (UI_H / 16)
 * |    Settings    |      |
 * |      Quit      |      |
 * |                |      |
 * +----------------+     -v-
 */

enum item {
	ITEM_PLAY,
	ITEM_SCORES,
	ITEM_SETTINGS,
	ITEM_QUIT,
	ITEM_LAST
};

struct menu {
	/* Title and its shadow */
	struct node title[2];

	/* Menu list and its items. */
	struct list_item items[ITEM_LAST];
	struct list list;
	struct coroutine coroutine;
};

static void
menu_entry(struct coroutine *self)
{
	struct texture texture;
	struct menu *menu;

	menu = MENU(self, coroutine);

	/* Title shadow. */
	ui_render(&texture, UI_FONT_TITLE, UI_PALETTE_SHADOW, "stris");
	node_wrap(&menu->title[0], &texture);

	/* Title foreground. */
	ui_render(&texture, UI_FONT_TITLE, UI_PALETTE_FG, "stris");
	node_wrap(&menu->title[1], &texture);

	menu->title[0].x  = menu->title[1].x = MENU_TITLE_X(menu);
	menu->title[0].y  = menu->title[1].y = MENU_TITLE_Y(menu);
	menu->title[0].x += 1;
	menu->title[0].y += 1;

	/* Main menu. */
	menu->items[ITEM_PLAY].text = "Play";
	menu->items[ITEM_SCORES].text = "Scores";
	menu->items[ITEM_SETTINGS].text = "Settings";
	menu->items[ITEM_QUIT].text = "Quit";
	menu->list.font = UI_FONT_MENU;
	menu->list.items = menu->items;
	menu->list.itemsz = ITEM_LAST;
	menu->list.y = 4 * (UI_H / 16);
	menu->list.w = UI_W;
	menu->list.h = 12 * (UI_H / 16);
	list_init(&menu->list);

	switch (list_wait(&menu->list)) {
	case ITEM_PLAY:
		break;
	case ITEM_SCORES:
		break;
	case ITEM_SETTINGS:
		break;
	case ITEM_QUIT:
		stris_quit();
		break;
	default:
		break;
	}
}

static void
menu_terminate(struct coroutine *self)
{
	struct menu *menu = MENU(self, coroutine);

	list_finish(&menu->list);

	for (size_t i = 0; i < 2; ++i)
		node_finish(&menu->title[i]);
}

void
menu_run(void)
{
	struct menu *menu;

	menu = alloc(1, sizeof (*menu));
	menu->coroutine.entry = menu_entry;
	menu->coroutine.terminate = menu_terminate;
	coroutine_init(&menu->coroutine);
}
