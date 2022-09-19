/*
 * menu.h -- menu helpers
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

#ifndef STRIS_MENU_H
#define STRIS_MENU_H

#include <stddef.h>

#include "ui.h"

enum key;

struct menu_item {
	// User editable fields.
	const char *text;
	enum ui_font font;

	// Private fields.
	int x;
	int y;
	int w;
	int h;
	int spent;
	int selected;
	unsigned long colorcur;
	unsigned long colordst;
};

struct menu {
	int halign;
	int valign;
	int x;
	int y;
	int w;
	int h;
	size_t selection;
	struct menu_item *items;
	size_t itemsz;
};

void
menu_init(struct menu *);

void
menu_reset(struct menu *);

int
menu_onkey(struct menu *, enum key);

void
menu_update(struct menu *, int);

void
menu_draw(const struct menu *);

#endif // !STRIS_MENU_H/
