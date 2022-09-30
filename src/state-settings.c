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
#include "list.h"
#include "sound.h"
#include "state-menu.h"
#include "state-settings.h"
#include "state.h"
#include "stris.h"
#include "sys.h"
#include "tex.h"
#include "ui.h"
#include "util.h"

enum menu {
	MENU_SOUND,
	MENU_MUSIC,
	MENU_PSYCHEDELIC,
	MENU_SCALE,
	MENU_LAST
};

static struct list_item items[] = {
	[MENU_SOUND]            = { .text = "Sounds"            },
	[MENU_MUSIC]            = { .text = "Music"             },
	[MENU_PSYCHEDELIC]      = { .text = "Psychedelic"       },
	[MENU_SCALE]            = { .text = "Scaling"           },
};

static struct list menu = {
	.font = UI_FONT_MENU_SMALL,
	.items = items,
	.itemsz = MENU_LAST,
	.halign = -1,
	.valign = -1,
	.p = 10
};

static void
draw_val(size_t index, const char *what)
{
	struct tex tex;

	ui_render(&tex, UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, what);
	tex_draw(&tex, UI_W - tex.w - 9, menu.items[index].y + 1);
	tex_finish(&tex);

	ui_render(&tex, UI_FONT_MENU_SMALL, menu.items[index].colorcur, what);
	tex_draw(&tex, UI_W - tex.w - 10, menu.items[index].y);
	tex_finish(&tex);
}

static void
init(void)
{
	list_init(&menu);
}

static void
resume(void)
{
	list_reset(&menu);
	list_select(&menu, 0);
}

static void
onkey(enum key key)
{
	list_onkey(&menu, key);

	switch (key) {
	case KEY_CANCEL:
		stris_switch(&state_menu);
		break;
	case KEY_SELECT:
		switch (menu.selection) {
		case MENU_SOUND:
			if ((sconf.sound = !sconf.sound))
				sound_init();
			else
				sound_finish();
			break;
		case MENU_MUSIC:
			sconf.music = !sconf.music;
			break;
		case MENU_PSYCHEDELIC:
			sconf.psychedelic = !sconf.psychedelic;
			break;
		case MENU_SCALE:
			sconf.scale = clamp((++sconf.scale % 3), 1, 2);
			ui_resize();
			break;
		default:
			break;
		}
		sys_conf_write();
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
	char str[16];

	ui_clear(UI_PALETTE_MENU_BG);
	ui_draw_background();
	list_draw(&menu);

	draw_val(0, sconf.sound ? "Yes" : "No");
	draw_val(1, sconf.music ? "Yes" : "No");
	draw_val(2, sconf.psychedelic ? "Yes" : "No");
	snprintf(str, sizeof (str), "%d", sconf.scale);
	draw_val(3, str);

	ui_present();
}

struct state state_settings = {
	.init = init,
	.resume = resume,
	.onkey = onkey,
	.update = update,
	.draw = draw
};
