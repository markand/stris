/*
 * shape.h -- all tetris shapes
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

#ifndef STRIS_SHAPE_H
#define STRIS_SHAPE_H

#include <stddef.h>

enum shape_rand {
	SHAPE_RAND_STANDARD = 7,
	SHAPE_RAND_EXTENDED = 10,
	SHAPE_RAND_NIGHTMARE = 12,
	SHAPE_RAND_MAX = SHAPE_RAND_NIGHTMARE
};

struct shape {
	int k;
	int x;
	int y;
	int o;
	int def[4][4][4];
};

/**
 * Returns the maximum number of rows that this shape consists of.
 *
 * This function only applies on the original first rotation.
 */
unsigned int
shape_max_rows(const struct shape *shape);

/**
 * Returns the maximum number of columns that this shape consists of.
 *
 * This function only applies on the original first rotation.
 */
unsigned int
shape_max_columns(const struct shape *shape);

void
shape_shuffle(struct shape *, size_t, enum shape_rand);

void
shape_rotate(struct shape *, int);

void
shape_move(struct shape *, int, int);

#endif /* STRIS_SHAPE_H */
