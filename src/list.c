/*
 * menu.c -- menu helpers
 *
 * Copyright (c) 2011-2023 David Demelier <markand@malikania.fr>
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
#include "list.h"
#include "tex.h"

#define GAP 4

static inline uint8_t
inc(uint8_t cmpcur, uint8_t cmpdst, int gap)
{
	if (gap >= fmax(cmpcur, cmpdst) - fmin(cmpcur, cmpdst))
		return cmpdst;

	return cmpcur + gap;
}

static inline uint32_t
add(const struct list_item *item, unsigned int shift)
{
	const uint8_t cmpcur = (item->colorcur >> shift) & 0xff;
	const uint8_t cmpdst = (item->colordst >> shift) & 0xff;

	return inc(cmpcur, cmpdst, cmpdst >= cmpcur ? +GAP : -GAP);
}

static void
take(struct list_item *item)
{
	item->selected = 1;
	item->spent = 0;
	item->colorcur = UI_PALETTE_MENU_SEL;
	item->colordst = UI_PALETTE_FG;
}

static void
discard(struct list_item *item)
{
	item->selected = 0;
	item->spent = 0;
	item->colorcur = UI_PALETTE_FG;
	item->colordst = UI_PALETTE_FG;
}

static void
update(struct list_item *item, int ticks)
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
draw(const struct list *l, const struct list_item *item)
{
	struct tex tex;

	ui_render(&tex, l->font, UI_PALETTE_SHADOW, item->text);
	tex_draw(&tex, item->x + 1, item->y + 1);
	tex_finish(&tex);

	ui_render(&tex, l->font, item->colorcur, item->text);
	tex_draw(&tex, item->x, item->y);
	tex_finish(&tex);
}

static void
halign(struct list *l)
{
	for (size_t i = 0; i < l->itemsz; ++i) {
		ui_clip(l->font, &l->items[i].w, &l->items[i].h,
		    "%s", l->items[i].text);

		switch (l->halign) {
		case -1:
			l->items[i].x = l->x + l->p;
			break;
		case 1:
			l->items[i].x = l->x + l->w - l->items[i].w - l->p;
			break;
		default:
			l->items[i].x = l->x + (l->w - l->items[i].w) / 2;
			break;
		}
	}
}

static void
valign(struct list *l)
{
	int ystart, vspace;

	switch (l->valign) {
	case -1:
		ystart = l->p;
		vspace = l->p;
		break;
	default:
		// Center.
		ystart = 0;
		vspace = (l->h - (l->items[0].h * l->itemsz)) / (l->itemsz + 1);
		break;
	}

	for (size_t i = 0; i < l->itemsz; ++i)
		l->items[i].y = l->y + ystart + (((i + 1) * vspace) + (i * l->items[0].h));
}

void
list_init(struct list *l)
{
	assert(l);

	halign(l);
	valign(l);
}

void
list_reset(struct list *l)
{
	assert(l);

	l->selection = 0;

	for (size_t i = 0; i < l->itemsz; ++i)
		discard(&l->items[i]);
}

void
list_select(struct list *l, size_t index)
{
	assert(l);
	assert(index < l->itemsz);

	discard(&l->items[l->selection]);
	take(&l->items[l->selection = index]);
}

int
list_onkey(struct list *l, enum key key)
{
	assert(l);

	size_t newsel = l->selection;
	int ret = 0;

	switch (key) {
	case KEY_UP:
		if (newsel == 0)
			newsel = l->itemsz - 1;
		else
			newsel--;
		break;
	case KEY_DOWN:
		if (newsel >= l->itemsz - 1)
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
	if (newsel != l->selection)
		list_select(l, newsel);

	return ret;
}

void
list_update(struct list *l, int ticks)
{
	assert(l);

	for (size_t i = 0; i < l->itemsz; ++i)
		update(&l->items[i], ticks);
}

void
list_draw(const struct list *l)
{
	assert(l);

	for (size_t i = 0; i < l->itemsz; ++i)
		draw(l, &l->items[i]);
}
