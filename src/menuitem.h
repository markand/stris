/*
 * menuitem.h -- menu item
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

#ifndef STRIS_MENUITEM_H
#define STRIS_MENUITEM_H

struct menuitem {
	int x;
	int y;
	int w;
	int h;
	int selected;
	int spent;
	unsigned long colorcur;
	unsigned long colordst;
	const char *text;
};

void
menuitem_init(struct menuitem *, const char *);

void
menuitem_select(struct menuitem *);

void
menuitem_unselect(struct menuitem *);

void
menuitem_move(struct menuitem *, int, int);

void
menuitem_update(struct menuitem *, int);

void
menuitem_draw(const struct menuitem *);

#endif /* !STRIS_MENUITEM_H */
