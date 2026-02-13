/*
 * state-mode.c -- mode selection
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

#include "list.h"
#include "state-menu.h"
#include "state-play.h"
#include "stris.h"
#include "ui.h"
#include "util.h"

#define STATE(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct state, Field))

struct state {
	struct list_item items[MODE_LAST];
	struct list list;
	struct coroutine coroutine;
};

static void
mode_entry(struct coroutine *self)
{
	struct state *state = STATE(self, coroutine);

	state->items[MODE_STANDARD].text = "standard";
	state->items[MODE_EXTENDED].text = "extended";
	state->items[MODE_NIGHTMARE].text = "nightmare";

	state->list.font = UI_FONT_MENU_SMALL;
	state->list.items = state->items;
	state->list.itemsz = LEN(state->items);
	state->list.w = UI_W;
	state->list.h = UI_H;
	list_init(&state->list);
	list_wait(&state->list);

	if (state->list.selection == (size_t)-1)
		menu_run();
	else
		play_run(state->list.selection);
}

static void
mode_terminate(struct coroutine *self)
{
	struct state *state = STATE(self, coroutine);

	list_finish(&state->list);
}

void
mode_run(void)
{
	struct state *state;

	state = alloc(1, sizeof (*state));
	state->coroutine.entry = mode_entry;
	state->coroutine.terminate = mode_terminate;
	coroutine_init(&state->coroutine);
}
