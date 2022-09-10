/*
 * board.c -- game main grid
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

#include <assert.h>
#include <string.h>

#include "board.h"
#include "shape.h"

void
board_clear(Board b)
{
	assert(b);

	memset(b, 0, BOARD_H * BOARD_W * sizeof (int));
}

void
board_pop(Board b, int y)
{
	memmove(b + 1, b, y * sizeof (b[y]));
}

int
board_check(const Board b, const struct shape *s)
{
	assert(b);
	assert(s);

	for (int r = 0; r < 4 && s->y + r < BOARD_H; ++r)
		for (int c = 0; c < 4 && s->x + c < BOARD_W; ++c)
			if (s->def[s->o][r][c] && b[s->y + r][s->x + c])
				return 0;

	return 1;
}

void
board_set(Board b, const struct shape *s)
{
	assert(b);
	assert(s);

	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			if (!b[s->y + r][s->x + c])
				b[s->y + r][s->x + c] = s->def[s->o][r][c];
}

void
board_unset(Board b, const struct shape *s)
{
	assert(b);
	assert(s);

	for (int r = 0; r < 4; ++r)
		for (int c = 0; c < 4; ++c)
			b[s->y + r][s->x + c] = 0;
}
