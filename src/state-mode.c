/*
 * state-mode.c -- mode selection
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
#include "state-mode.h"
#include "state-play.h"
#include "state.h"
#include "stris.h"
#include "ui.h"
#include "util.h"

static struct list_item items[] = {
	[STRIS_MODE_STANDARD]   = { .text = "standard"  },
	[STRIS_MODE_EXTENDED]   = { .text = "extended"  },
	[STRIS_MODE_NIGHTMARE]  = { .text = "nightmare" }
};

static struct list menu = {
	.font = UI_FONT_MENU_SMALL,
	.items = items,
	.itemsz = LEN(items),
	.w = UI_W,
	.h = UI_H
};

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
onkey(enum key key, int state)
{
	if (!state)
		return;

	if (list_onkey(&menu, key)) {
		state_play_mode = menu.selection;
		stris_switch(&state_play);
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
	list_draw(&menu);
	ui_present();
}

struct state state_mode = {
	.init = init,
	.resume = resume,
	.onkey = onkey,
	.update = update,
	.draw = draw
};
