/*
 * state-menu.c -- main menu
 *
 * Copyright (c) 2011-2024 David Demelier <markand@malikania.fr>
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
#include "list.h"
#include "state-mode.h"
#include "state-scores.h"
#include "state-settings.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"

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

enum state_menu {
	STATE_MENU_PLAY,
	STATE_MENU_SCORES,
	STATE_MENU_SETTINGS,
	STATE_MENU_QUIT,
	STATE_MENU_LAST
};

static struct {
	struct tex tex[2];
	int x;
	int y;
	int h;
} title;

static struct list_item items[STATE_MENU_LAST] = {
	[STATE_MENU_PLAY]       = { .text = "Play"      },
	[STATE_MENU_SCORES]     = { .text = "Scores"    },
	[STATE_MENU_SETTINGS]   = { .text = "Settings"  },
	[STATE_MENU_QUIT]       = { .text = "Quit"      }
};

static struct list menu = {
	.font = UI_FONT_MENU,
	.items = items,
	.itemsz = STATE_MENU_LAST
};

static void
init(void)
{
	// Title.
	ui_render(&title.tex[0], UI_FONT_TITLE, UI_PALETTE_FG, "stris");
	ui_render(&title.tex[1], UI_FONT_TITLE, UI_PALETTE_SHADOW, "stris");
	title.x = (UI_W - title.tex[0].w) / 2;
	title.h = (UI_H * 4) / 16;
	title.y = (title.h - title.tex[0].h) / 2;

	// Main menu.
	menu.y = title.h;
	menu.w = UI_W;
	menu.h = UI_H - title.h;
	list_init(&menu);
}

static void
resume(void)
{
	list_reset(&menu);
	list_select(&menu, 0);
}

static void
onkey(enum key key, int state)
{
	if (!state || !list_onkey(&menu, key))
		return;

	switch (menu.selection) {
	case STATE_MENU_PLAY:
		stris_switch(&state_mode);
		break;
	case STATE_MENU_SCORES:
		stris_switch(&state_scores);
		break;
	case STATE_MENU_SETTINGS:
		stris_switch(&state_settings);
		break;
	case STATE_MENU_QUIT:
		stris_quit();
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
	list_update(&menu, ticks);
	ui_update_background(UI_PALETTE_MENU_BG, ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	ui_draw_background();

	tex_draw(&title.tex[1], title.x + 1, title.y + 1);
	tex_draw(&title.tex[0], title.x, title.y);

	list_draw(&menu);
	ui_present();
}

static void
finish(void)
{
	tex_finish(&title.tex[0]);
	tex_finish(&title.tex[1]);
}

struct state state_menu = {
	.init = init,
	.resume = resume,
	.onkey = onkey,
	.update = update,
	.draw = draw,
	.finish = finish
};
