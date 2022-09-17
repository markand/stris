/*
 * state-splash.c -- simple advertising splash screen
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

#include "sound.h"
#include "state-menu.h"
#include "state-splash.h"
#include "state.h"
#include "stris.h"
#include "tex.h"
#include "ui.h"

static int played;
static int spent;
static struct tex title;

static void
init(void)
{
	ui_render(&title, UI_FONT_SPLASH, UI_PALETTE_FG, "malikania");
}

static void
update(int ticks)
{
	spent += ticks;

	//
	// We wait a little before playing sound because the window may take
	// time to open.
	//
	if (spent >= 100 && !played) {
		played = 1;
		sound_play(SOUND_CHIME);
	}
	if (spent >= 2000)
		stris_switch(&state_menu);
}

static void
draw(void)
{
	ui_clear(UI_PALETTE_SPLASH_BG);
	tex_draw(&title, (UI_W - title.w) / 2, (UI_H - title.h) / 2);
	ui_present();
}

static void
finish(void)
{
	tex_finish(&title);
}

struct state state_splash = {
	.init = init,
	.update = update,
	.draw = draw,
	.finish = finish
};
