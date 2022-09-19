/*
 * menu.c -- menu helpers
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
#include <math.h>

#include "key.h"
#include "menu.h"
#include "tex.h"

#define PAD 10
#define GAP 4

static inline unsigned int
inc(unsigned int cmpcur, unsigned int cmpdst, int gap)
{
	if (gap >= fmax(cmpcur, cmpdst) - fmin(cmpcur, cmpdst))
		return cmpdst;

	return cmpcur + gap;
}

static inline unsigned int
add(const struct menu_item *item, unsigned int shift)
{
	const unsigned int cmpcur = (item->colorcur >> shift) & 0xff;
	const unsigned int cmpdst = (item->colordst >> shift) & 0xff;

	return inc(cmpcur, cmpdst, cmpdst >= cmpcur ? +GAP : -GAP);
}

static void
take(struct menu_item *item)
{
	item->selected = 1;
	item->spent = 0;
	item->colorcur = UI_PALETTE_MENU_SEL;
	item->colordst = UI_PALETTE_FG;
}

static void
discard(struct menu_item *item)
{
	item->selected = 0;
	item->spent = 0;
	item->colorcur = UI_PALETTE_FG;
	item->colordst = UI_PALETTE_FG;
}

static void
update(struct menu_item *item, int ticks)
{
	if (!item->selected)
		return;

	item->spent += ticks;

	if (item->spent >= 30) {
		item->spent = 0;

		// Reached color destination, change direction.
		if (item->colorcur == item->colordst)
			item->colordst = item->colordst == UI_PALETTE_FG
				? UI_PALETTE_MENU_SEL
				: UI_PALETTE_FG;

		item->colorcur = add(item, 24) << 24 |
		                 add(item, 16) << 16 |
		                 add(item, 8)  << 8 |
		                 0xff;
	}
}

static void
draw(const struct menu_item *item)
{
	struct tex tex;

	ui_render(&tex, item->font, UI_PALETTE_SHADOW, item->text);
	tex_draw(&tex, item->x + 1, item->y + 1);
	tex_finish(&tex);

	ui_render(&tex, item->font, item->colorcur, item->text);
	tex_draw(&tex, item->x, item->y);
	tex_finish(&tex);
}

static void
halign(struct menu *m)
{
	for (size_t i = 0; i < m->itemsz; ++i) {
		ui_clip(m->items[i].font, &m->items[i].w, &m->items[i].h,
		    "%s", m->items[i].text);

		switch (m->halign) {
		case -1:
			m->items[i].x = m->x + PAD;
			break;
		case 1:
			m->items[i].x = m->x + m->w - m->items[i].w - PAD;
			break;
		default:
			m->items[i].x = m->x + (m->w - m->items[i].w) / 2;
			break;
		}
	}
}

static void
valign(struct menu *m)
{
	int ystart, vspace;

	switch (m->valign) {
	case -1:
		ystart = PAD;
		vspace = PAD;
		break;
	default:
		// Center.
		ystart = 0;
		vspace = (m->h - (m->items[0].h * m->itemsz)) / (m->itemsz + 1);
		break;
	}

	for (size_t i = 0; i < m->itemsz; ++i)
		m->items[i].y = m->y + ystart + (((i + 1) * vspace) + (i * m->items[0].h));
}

void
menu_init(struct menu *m)
{
	assert(m);

	halign(m);
	valign(m);
}

void
menu_reset(struct menu *m)
{
	assert(m);

	m->selection = 0;

	for (size_t i = 0; i < m->itemsz; ++i)
		discard(&m->items[i]);

	take(&m->items[0]);
}

int
menu_onkey(struct menu *m, enum key key)
{
	assert(m);

	size_t newsel = m->selection;
	int ret = 0;

	switch (key) {
	case KEY_UP:
		if (newsel == 0)
			newsel = m->itemsz - 1;
		else
			newsel--;
		break;
	case KEY_DOWN:
		if (newsel >= m->itemsz - 1)
			newsel = 0;
		else
			newsel++;
		break;
	case KEY_SELECT:
		ret = 1;
		break;
	default:
		break;
	}

	// We also need to reset old one.
	if (newsel != m->selection) {
		discard(&m->items[m->selection]);
		take(&m->items[m->selection = newsel]);
	}

	return ret;
}

void
menu_update(struct menu *m, int ticks)
{
	assert(m);

	for (size_t i = 0; i < m->itemsz; ++i)
		update(&m->items[i], ticks);
}

void
menu_draw(const struct menu *m)
{
	assert(m);

	for (size_t i = 0; i < m->itemsz; ++i)
		draw(&m->items[i]);
}
