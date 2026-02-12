/*
 * node.c -- graphical object on scene
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

#include <assert.h>
#include <stdlib.h>

#include "node.h"
#include "stris.h"
#include "texture.h"
#include "util.h"

void
node_init(struct node *node)
{
	assert(node);
	assert(node->texture);

	struct node **slot = NULL;

	node->own = 0;

	for (size_t i = 0; i < LEN(stris.nodes); ++i) {
		if (!stris.nodes[i]) {
			slot = &stris.nodes[i];
			break;
		}
	}

	if (!slot)
		die("abort: node space exceeded\n");

	*slot = node;
}

void
node_wrap(struct node *node, struct texture *texture)
{
	assert(node);
	assert(texture);

	node->texture = allocdup(texture, sizeof (*texture));
	node_init(node);
	node->own = 1;

	/* Invalidate to secure texture_finish. */
	texture->handle = NULL;
	texture->w = texture->h = 0;
}

void
node_render(struct node *node)
{
	assert(node);

	if (!node->hide)
		texture_render(node->texture, node->x, node->y);
}

void
node_finish(struct node *node)
{
	assert(node);

	for (size_t i = 0; i < LEN(stris.nodes); ++i) {
		if (stris.nodes[i] == node) {
			stris.nodes[i] = NULL;
			break;
		}
	}

	if (node->own) {
		texture_finish(node->texture);
		free(node->texture);
		node->texture = NULL;
		node->own = 0;
	}
}
