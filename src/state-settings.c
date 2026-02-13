/*
 * state-settings.c -- game settings
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

#include <stdarg.h>
#include <stdio.h>

#include "list.h"
#include "sound.h"
#include "state-menu.h"
#include "stris.h"
#include "sys.h"
#include "texture.h"
#include "ui.h"
#include "util.h"

#define SETTINGS(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct settings, Field))

enum item {
	ITEM_SOUND,
	ITEM_PSYCHEDELIC,
	ITEM_SCALE,
	ITEM_LAST
};

struct value {
	struct texture texture[2];
	struct node node[2];
};

struct settings {
	/* Values shown for each element. */
	struct value values[ITEM_LAST];

	/* Menu list and its items. */
	struct list_item items[ITEM_LAST];
	struct list list;
	struct coroutine configurator;

	/* Coroutine for escaping. */
	struct coroutine handler;
};

/*
 * Convert a value settings as human readable format.
 *
 * Re-render the texture and adjust its position on screen on its sibling menu
 * item.
 */
static void
settings_valuize(struct settings *settings, size_t row, const char *fmt, ...)
{
	struct value *value;
	char str[16] = {};
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(str, sizeof (str), fmt, ap);
	va_end(ap);

	value = &settings->values[row];

	ui_render(&value->texture[0], UI_FONT_MENU_SMALL, UI_PALETTE_SHADOW, str);
	ui_render(&value->texture[1], UI_FONT_MENU_SMALL, UI_PALETTE_FG, str);

	/* Register the nodes if it was not already done. */
	value->node[1].x = UI_W - value->texture[0].w - settings->list.p;
	value->node[0].x = value->node[1].x + 1;

	value->node[1].y = settings->items[row].node->y;
	value->node[0].y = value->node[1].y + 1;

}

static void
settings_configurator_entry(struct coroutine *self)
{
	struct settings *settings = SETTINGS(self, configurator);

	settings->items[ITEM_SOUND].text = "Sounds";
	settings->items[ITEM_PSYCHEDELIC].text = "Psychedelic";
	settings->items[ITEM_SCALE].text = "Scaling";

	settings->list.font = UI_FONT_MENU_SMALL;
	settings->list.items = settings->items;
	settings->list.itemsz = ITEM_LAST;
	settings->list.halign = -1;
	settings->list.valign = -1;
	settings->list.w = UI_W;
	settings->list.h = UI_H;
	settings->list.p = 10;
	list_init(&settings->list);

	for (size_t i = 0; i < ITEM_LAST; ++i) {
		settings->values[i].node[0].texture = &settings->values[i].texture[0];
		settings->values[i].node[1].texture = &settings->values[i].texture[1];
		node_init(&settings->values[i].node[0]);
		node_init(&settings->values[i].node[1]);
	}

	for (;;) {
		/* Re-render values. */
		settings_valuize(settings, 0, sconf.sound ? "Yes" : "No");
		settings_valuize(settings, 1, sconf.psychedelic ? "Yes" : "No");
		settings_valuize(settings, 2, "%d", sconf.scale);

		switch (list_wait(&settings->list)) {
		case ITEM_SOUND:
			if ((sconf.sound = !sconf.sound))
				sound_init();
			else
				sound_finish();
			break;
		case ITEM_PSYCHEDELIC:
			sconf.psychedelic = !sconf.psychedelic;
			break;
		case ITEM_SCALE:
			sconf.scale = clamp((++sconf.scale % 3), 1, 2);
			ui_resize();
			break;
		default:
			break;
		}

		sys_conf_write();
	}
}

static void
settings_configurator_terminate(struct coroutine *self)
{
	struct settings *settings = SETTINGS(self, configurator);

	list_finish(&settings->list);

	for (size_t i = 0; i < ITEM_LAST; ++i) {
		node_finish(&settings->values[i].node[0]);
		node_finish(&settings->values[i].node[1]);
		texture_finish(&settings->values[i].texture[0]);
		texture_finish(&settings->values[i].texture[1]);
	}
}

static void
settings_handler_entry(struct coroutine *)
{
	while (!(stris_pressed() & KEY_CANCEL))
		continue;

	menu_run();
}

static void
settings_handler_terminate(struct coroutine *self)
{
	struct settings *settings = SETTINGS(self, handler);

	coroutine_finish(&settings->configurator);
}

void
settings_run(void)
{
	struct settings *settings;

	settings = alloc(1, sizeof (*settings));

	/* Long living menu settings */
	settings->configurator.entry = settings_configurator_entry;
	settings->configurator.terminate = settings_configurator_terminate;
	coroutine_init(&settings->configurator);

	/* State handler */
	settings->handler.entry = settings_handler_entry;
	settings->handler.terminate = settings_handler_terminate;
	coroutine_init(&settings->handler);
}
