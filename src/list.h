/*
 * list.h -- list and menu helpers
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

#ifndef STRIS_LIST_H
#define STRIS_LIST_H

/**
 * \file list.h
 * \brief List and menu helpers.
 */

#include <stddef.h>
#include <stdint.h>

#include "coroutine.h"
#include "node.h"
#include "ui.h"

/**
 * \struct list_item
 * \brief List menu item.
 */
struct list_item {
	/**
	 * (init)
	 *
	 * Text to render.
	 */
	const char *text;

	/* Rendering nodes. */
	struct node node[2];
};

/**
 * \struct list
 * \brief Interactive menu list.
 */
struct list {
	/**
	 * (init)
	 *
	 * Font to use for rendering.
	 */
	enum ui_font font;

	/**
	 * (init)
	 *
	 * Horizontal alignment.
	 *
	 * < 0: align left
	 * = 0: align center
	 * > 0: align right
	 */
	int halign;

	/**
	 * (init)
	 *
	 * Similar to ::list::halign but vertically.
	 */
	int valign;

	/**
	 * (init)
	 *
	 * Position in x.
	 */
	int x;

	/**
	 * (init)
	 *
	 * Position in y.
	 */
	int y;

	/**
	 * (init)
	 *
	 * List width.
	 */
	unsigned int w;

	/**
	 * (init)
	 *
	 * List height.
	 */
	unsigned int h;

	/**
	 * (optional)
	 *
	 * Padding between elements.
	 */
	unsigned int p;

	/**
	 * (init)
	 *
	 * Items to display.
	 */
	struct list_item *items;

	/**
	 * (init)
	 *
	 * Number of elements in ::list::items.
	 */
	size_t itemsz;

	size_t selection;               /* currently selected */
	struct coroutine colorizer;     /* hover glower */
	struct coroutine selector;      /* list selector */
};

/**
 * Initialize the list menu.
 */
void
list_init(struct list *list);

/**
 * Yield the calling coroutine until an element has been selected.
 *
 * Since the selection happens in a different coroutine, it's not mandatory to
 * use this function and use ::list::selection directly if yielding is an issue
 * in the calling coroutine.
 */
size_t
list_wait(struct list *);

/**
 * Destroy the list menu.
 */
void
list_finish(struct list *);

#endif /* !STRIS_LIST_H */
