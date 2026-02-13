/*
 * state-splash.c -- simple advertising splash screen
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

#include <stdlib.h>

#include "coroutine.h"
#include "node.h"
#include "sound.h"
#include "state-menu.h"
#include "state-splash.h"
#include "stris.h"
#include "texture.h"
#include "ui.h"
#include "util.h"

#define SPLASH(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct splash, Field))

struct splash {
	struct node background;
	struct node title;
	struct coroutine coroutine;
};

static void
splash_entry(struct coroutine *self)
{
	struct texture texture;
	struct splash *splash;

	splash = SPLASH(self, coroutine);

	/* Background. */
	texture_init(&texture, UI_W, UI_H);

	UI_BEGIN(&texture);
	ui_clear(UI_PALETTE_SPLASH_BG);
	UI_END();

	node_wrap(&splash->background, &texture);

	/* Center brag. */
	ui_printf(&texture, UI_FONT_SPLASH, UI_PALETTE_FG, "malikania");

	node_wrap(&splash->title, &texture);
	splash->title.x = (UI_W - splash->title.texture->w) / 2;
	splash->title.y = (UI_H - splash->title.texture->h) / 2;

	/* Wait for splash to show up. */
	sound_play(SOUND_CHIME);
	coroutine_sleep(1500);

	/* Switch to menu. */
	menu_run();
}

static void
splash_terminate(struct coroutine *self)
{
	struct splash *splash = SPLASH(self, coroutine);

	node_finish(&splash->title);
	node_finish(&splash->background);

	free(splash);
}

void
splash_run(void)
{
	struct splash *splash;

	splash = alloc(1, sizeof (*splash));
	splash->coroutine.entry = splash_entry;
	splash->coroutine.terminate = splash_terminate;
	coroutine_init(&splash->coroutine);
}
