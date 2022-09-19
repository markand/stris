/*
 * state-menu.c -- main menu
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
#include "menuitem.h"
#include "state-menu.h"
#include "state-play.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

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

static struct {
	struct tex tex[2];
	int x;
	int y;
	int h;
} title;

static struct {
	struct menuitem items[4];
	int sel;
} menu;

static void
handle_select(void)
{
	switch (menu.sel) {
	case 0:
		stris_switch(&state_play);
		break;
	case 3:
		stris_quit();
		break;
	default:
		break;
	}
}

static void
init(void)
{
	// Title.
	ui_render(&title.tex[0], UI_FONT_TITLE, UI_PALETTE_FG, "stris");
	ui_render(&title.tex[1], UI_FONT_TITLE, UI_PALETTE_SHADOW, "stris");
	title.x = (UI_W - title.tex[0].w) / 2;
	title.h = (UI_H * 4) / 16;
	title.y = (title.h - title.tex[0].h) / 2;

	// Menu items.
	menuitem_init(&menu.items[0], "Play");
	menuitem_init(&menu.items[1], "Scores");
	menuitem_init(&menu.items[2], "Settings");
	menuitem_init(&menu.items[3], "Quit");

	// Vertically align items.
	valign(UI_H - title.h, menu.items[0].h, title.h, (int *[]){
		&menu.items[0].y,
		&menu.items[1].y,
		&menu.items[2].y,
		&menu.items[3].y,
		NULL
	});

	// Center them horizontally and add vertical padding.
	for (size_t i = 0; i < LEN(menu.items); ++i)
		hcenter(UI_W, menu.items[i].w, &menu.items[i].x);
}

static void
resume(void)
{
	menu.sel = 0;
	menuitem_select(&menu.items[0]);
}

static void
onkey(enum key key)
{
	int newsel = menu.sel;

	switch (key) {
	case KEY_UP:
		if (newsel == 0)
			newsel = 3;
		else
			newsel--;
		break;
	case KEY_DOWN:
		if (newsel >= 3)
			newsel = 0;
		else
			newsel++;
		break;
	case KEY_SELECT:
		handle_select();
		break;
	default:
		break;
	}

	// We also need to reset old one.
	if (newsel != menu.sel) {
		menuitem_unselect(&menu.items[menu.sel]);
		menuitem_select(&menu.items[menu.sel = newsel]);
	}
}

static void
update(int ticks)
{
	for (size_t i = 0; i < LEN(menu.items); ++i)
		menuitem_update(&menu.items[i], ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);

	tex_draw(&title.tex[1], title.x + 1, title.y + 1);
	tex_draw(&title.tex[0], title.x, title.y);

	for (size_t i = 0; i < LEN(menu.items); ++i)
		menuitem_draw(&menu.items[i]);

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
