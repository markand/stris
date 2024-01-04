/*
 * list.h -- list and menu helpers
 *
 * Copyright (c) 2011-2024 David Demelier <markand@malikania.fr>
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

#ifndef STRIS_LIST_H
#define STRIS_LIST_H

#include <stddef.h>
#include <stdint.h>

#include "ui.h"

enum key;

struct list_item {
	// User editable fields.
	const char *text;

	// Private fields.
	int x;
	int y;
	int w;
	int h;
	int spent;
	int selected;
	uint32_t colorcur;
	uint32_t colordst;
};

struct list {
	enum ui_font font;
	int halign;
	int valign;
	int x;
	int y;
	int w;
	int h;
	int p;
	size_t selection;
	struct list_item *items;
	size_t itemsz;
};

void
list_init(struct list *);

void
list_reset(struct list *);

void
list_select(struct list *, size_t);

int
list_onkey(struct list *, enum key);

void
list_update(struct list *, int);

void
list_draw(const struct list *);

#endif // !STRIS_LIST_H/
