/*
 * shape.h -- all tetris shapes
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

#ifndef STRIS_SHAPE_H
#define STRIS_SHAPE_H

enum shape_kind {
	SHAPE_NONE,
	SHAPE_O,
	SHAPE_I,
	SHAPE_S,
	SHAPE_Z,
	SHAPE_L,
	SHAPE_J,
	SHAPE_T,
	SHAPE_C,
	SHAPE_X,
	SHAPE_P,
	SHAPE_D,
	SHAPE_RANDOM_STD = 7,
	SHAPE_RANDOM_EXT = 11
};

struct shape {
	enum shape_kind k;
	int x;
	int y;
	int o;
	int def[4][4][4];
};

void
shape_select(struct shape *, enum shape_kind);

void
shape_rotate(struct shape *, int);

void
shape_move(struct shape *, int, int);

#endif /* STRIS_SHAPE_H */
