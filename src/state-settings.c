/*
 * state-settings.c -- game settings
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
#include "menu.h"
#include "state-menu.h"
#include "state-settings.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

enum state_menu {
	STATE_MENU_SOUND,
	STATE_MENU_MUSIC,
	STATE_MENU_LAST
};

static struct menu_item items[] = {
	[STATE_MENU_SOUND] = {
		.text = "Sounds",
		.font = UI_FONT_MENU_SMALL
	},
	[STATE_MENU_MUSIC] = {
		.text = "Music",
		.font = UI_FONT_MENU_SMALL
	}
};

static struct menu menu = {
	.items = items,
	.itemsz = STATE_MENU_LAST,
	.halign = -1,
	.valign = -1
};

static void
draw_val(size_t index, int val)
{
	struct tex tex;
	const char *text = val ? "Yes" : "No";

	// TODO: not huge fan of this.
	ui_render(&tex, UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, text);
	tex_draw(&tex, UI_W - tex.w - 9, menu.items[index].y + 1);
	tex_finish(&tex);

	ui_render(&tex, UI_FONT_MENU_SMALL, menu.items[index].colorcur, text);
	tex_draw(&tex, UI_W - tex.w - 10, menu.items[index].y);
	tex_finish(&tex);
}

static void
init(void)
{
	menu_init(&menu);
}

static void
resume(void)
{
	menu_reset(&menu);
}

static void
onkey(enum key key)
{
	menu_onkey(&menu, key);

	// TODO: disable playback here.
	switch (key) {
	case KEY_CANCEL:
		stris_switch(&state_menu);
		break;
	case KEY_SELECT:
		switch (menu.selection) {
		case STATE_MENU_SOUND:
			sconf.sound = !sconf.sound;
			break;
		case STATE_MENU_MUSIC:
			sconf.music = !sconf.music;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

static void
update(int ticks)
{
	menu_update(&menu, ticks);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_MENU_BG);
	menu_draw(&menu);

	draw_val(0, sconf.sound);
	draw_val(1, sconf.music);

	ui_present();
}

struct state state_settings = {
	.init = init,
	.resume = resume,
	.onkey = onkey,
	.update = update,
	.draw = draw
};