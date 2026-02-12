/*
 * node.h -- graphical object on scene
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

#ifndef STRIS_NODE_H
#define STRIS_NODE_H

/**
 * \file node.h
 * \brief Graphical object on scene.
 */

struct texture;

/**
 * \struct node
 * \brief Graphical texture to be drawn on scene.
 */
struct node {
	/**
	 * (read-write)
	 *
	 * Texture to render.
	 */
	struct texture *texture;

	/**
	 * (read-write)
	 *
	 * Position in x.
	 */
	int x;

	/**
	 * (read-write)
	 *
	 * Position in y.
	 */
	int y;

	/**
	 * (read-write)
	 *
	 * Non-zero to skip texture when rendering.
	 */
	int hide;

	/* Non-zero if texture is owned by node. */
	int own;
};

/**
 * Initialize a rendering node and attach it to the game window.
 */
void
node_init(struct node *node);

/**
 * Convenient function that takes ownership of the texture and will destroy
 * it when the node is destroyed.
 *
 * The texture content is moved internally and its content reset. Caller can
 * discard the object and ::texture_finish is no-op.
 *
 * \param texture the texture to borrow (not NULL)
 */
void
node_wrap(struct node *node, struct texture *texture);

/**
 * Render the node on the screen.
 */
void
node_render(struct node *node);

/**
 * Remove node from stris main loop and destroy it.
 *
 * If the node owns the texture (as if ::node_wrap) was used, the underlying
 * texture is destroyed as well.
 */
void
node_finish(struct node *node);

#endif /* !STRIS_NODE_H */
