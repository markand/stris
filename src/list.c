/*
 * menu.c -- menu helpers
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

#include <stdio.h>

#include <assert.h>
#include <math.h>

#include "texture.h"
#include "list.h"
#include "stris.h"
#include "util.h"

#define LIST(Ptr, Field) \
        (CONTAINER_OF(Ptr, struct list, Field))

#define GAP 4

static inline uint8_t
inc(uint8_t cmpcur, uint8_t cmpdst, int gap)
{
	if (gap >= fmax(cmpcur, cmpdst) - fmin(cmpcur, cmpdst))
		return cmpdst;

	return cmpcur + gap;
}

static inline uint32_t
add(uint32_t cur, uint32_t dst, unsigned int shift)
{
	const uint8_t cmpcur = (cur >> shift) & 0xff;
	const uint8_t cmpdst = (dst >> shift) & 0xff;

	return inc(cmpcur, cmpdst, cmpdst >= cmpcur ? +GAP : -GAP);
}

static void
setup(struct list *list)
{
	struct list_item *li;
	struct texture texture;

	for (size_t i = 0; i < list->itemsz; ++i) {
		li = &list->items[i];

		ui_printf_shadowed(&texture, list->font, UI_PALETTE_FG, "%s", li->text);
		node_wrap(&li->node, &texture);
	}
}

static void
halign(struct list *l)
{
	for (size_t i = 0; i < l->itemsz; ++i) {
		switch (l->halign) {
		case -1:
			l->items[i].node.x = l->x + l->p;
			break;
		case 1:
			l->items[i].node.x = l->x + l->w - l->items[i].node.texture->w - l->p;
			break;
		default:
			l->items[i].node.x = l->x + (l->w - l->items[i].node.texture->w) / 2;
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
		ystart = 0;
		vspace = (l->h - (l->items[0].node.texture->h * l->itemsz)) / (l->itemsz + 1);
		break;
	}

	for (size_t i = 0; i < l->itemsz; ++i)
		l->items[i].node.y = l->y + ystart + (((i + 1) * vspace) + (i * l->items[0].node.texture->h));
}

static void
list_colorizer_entry(struct coroutine *self)
{
	struct list_item *li;
	struct list *list;
	uint32_t color, target;

	list = LIST(self, colorizer);

	color = UI_PALETTE_MENU_LOW;
	target = UI_PALETTE_MENU_HIGH;

	for (;;) {
		coroutine_sleep(30);

		/* Reached color target, change direction. */
		if (color == target)
			target = target == UI_PALETTE_MENU_HIGH ? UI_PALETTE_MENU_LOW : UI_PALETTE_MENU_HIGH;

		color  = add(color, target, 24) << 24 |
		         add(color, target, 16) << 16 |
		         add(color, target, 8)  << 8;
		color |= 0xff;

		for (size_t i = 0; i < list->itemsz; ++i) {
			li = &list->items[i];

			if (list->selection == i)
				texture_color_blend(li->node.texture, color);
			else
				texture_color_blend(li->node.texture, UI_PALETTE_FG);
		}
	}
}

static void
list_colorizer_terminate(struct coroutine *self)
{
	struct list *list = LIST(self, colorizer);

	for (size_t i = 0; i < list->itemsz; ++i)
		node_finish(&list->items[i].node);
}

static void
list_selector_entry(struct coroutine *self)
{
	struct list *list;
	enum key keys;

	list = LIST(self, selector);

	for (;;) {
		keys = stris_pressed();

		if (keys & KEY_UP) {
			if (list->selection == 0)
				list->selection = list->itemsz - 1;
			else
				list->selection -= 1;
		} else if (keys & KEY_DOWN) {
			if (list->selection >= list->itemsz - 1)
				list->selection = 0;
			else
				list->selection += 1;
		}
	}
}

void
list_init(struct list *list)
{
	assert(list);
	assert(list->items);
	assert(list->w);
	assert(list->h);

	setup(list);
	halign(list);
	valign(list);

	/* Colorizer on selected items. */
	list->colorizer.entry = list_colorizer_entry;
	list->colorizer.terminate = list_colorizer_terminate;
	coroutine_init(&list->colorizer);

	/* Selector using keys. */
	list->selector.entry = list_selector_entry;
	coroutine_init(&list->selector);
}

size_t
list_wait(struct list *list)
{
	assert(list);

	enum key keys;

	for (;;) {
		keys = stris_pressed();

		if (keys & KEY_SELECT)
			break;
		if (keys & KEY_CANCEL) {
			list->selection = (size_t)-1;
			break;
		}
	}

	return list->selection;
}

void
list_finish(struct list *l)
{
	assert(l);

	coroutine_finish(&l->selector);
	coroutine_finish(&l->colorizer);
}
