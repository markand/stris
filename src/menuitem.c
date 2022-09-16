/*
 * menuitem.c -- menu item
 *
 * Copyright (c) 2011-2021 David Demelier <markand@malikania.fr>
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
#include <string.h>

#include "menuitem.h"
#include "tex.h"
#include "ui.h"

#define GAP 4

static inline unsigned int
inc(unsigned int cmpcur, unsigned int cmpdst, int gap)
{
	if (gap >= fmax(cmpcur, cmpdst) - fmin(cmpcur, cmpdst))
		return cmpdst;

	return cmpcur + gap;
}

static inline unsigned int
add(const struct menuitem *m, unsigned int shift)
{
	const unsigned int cmpcur = (m->colorcur >> shift) & 0xff;
	const unsigned int cmpdst = (m->colordst >> shift) & 0xff;

	return inc(cmpcur, cmpdst, cmpdst >= cmpcur ? +GAP : -GAP);
}

void
menuitem_init(struct menuitem *m, const char *text)
{
	assert(m);

	memset(m, 0, sizeof (*m));
	m->text = text;
	m->colorcur = UI_PALETTE_FG;
	m->colordst = UI_PALETTE_FG;;
	ui_clip(UI_FONT_MENU, &m->w, &m->h, m->text);
}

void
menuitem_select(struct menuitem *m)
{
	assert(m);

	m->selected = 1;
	m->spent = 0;
	m->colorcur = UI_PALETTE_MENU_SEL;
	m->colordst = UI_PALETTE_FG;
}

void
menuitem_unselect(struct menuitem *m)
{
	assert(m);

	m->selected = 0;
	m->spent = 0;
	m->colorcur = UI_PALETTE_FG;
	m->colordst = UI_PALETTE_FG;;
}

void
menuitem_move(struct menuitem *m, int x, int y)
{
	assert(m);

	m->x = x;
	m->y = y;
}

void
menuitem_update(struct menuitem *m, int ticks)
{
	assert(m);

	if (!m->selected)
		return;

	m->spent += ticks;

	if (m->spent >= 30) {
		m->spent = 0;

		// Reached color destination, change direction.
		if (m->colorcur == m->colordst)
			m->colordst = m->colordst == UI_PALETTE_FG
				? UI_PALETTE_MENU_SEL
				: UI_PALETTE_FG;

		m->colorcur = add(m, 24) << 24 |
		              add(m, 16) << 16 |
		              add(m, 8) << 8 |
		              0xff;
	}
}

void
menuitem_draw(const struct menuitem *m)
{
	assert(m);

	struct tex tex;

	ui_render(&tex, UI_FONT_MENU, UI_PALETTE_SHADOW, m->text);
	tex_draw(&tex, m->x + 1, m->y + 1);
	tex_finish(&tex);

	ui_render(&tex, UI_FONT_MENU, m->colorcur, m->text);
	tex_draw(&tex, m->x, m->y);
	tex_finish(&tex);
}
