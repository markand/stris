/*
 * shape.c -- all tetris shapes
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
#include <string.h>

#include "shape.h"
#include "util.h"

static const int shapes[][4][4][4] = {
	// {{{ Standard

	// Rotation is based on SRS.
	// https://tetris.fandom.com/wiki/SRS
	{
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 }
		}
	},

	{
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 1 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
	},

	{
		{
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		}
	},
	// }}}

	// {{{ Extended.
	{
		{
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 1 },
			{ 0, 0, 1, 1 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		}
	},

	{
		{
			{ 0, 1, 1, 1 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 }
		},
		{
			{ 0, 1, 1, 1 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 }
		},
	},
	// }}}

	// {{{ Nightmare
	{
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
		},
	},

	{
		{
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
		},
	},
	// }}}
};

unsigned int
shape_max_rows(const struct shape *shape)
{
	assert(shape);

	unsigned int rows = 0;

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			if (shape->def[0][r][c]) {
				rows++;
				break;
			}
		}
	}

	return rows;
}

unsigned int
shape_max_columns(const struct shape *shape)
{
	assert(shape);

	unsigned int columns = 0;

	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			if (shape->def[0][r][c]) {
				columns++;
				break;
			}
		}
	}

	return columns;
}

unsigned int
shape_gap_rows(const struct shape *shape)
{
	assert(shape);

	unsigned int blanks = 0;

	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			if (shape->def[0][r][c])
				goto exit;
		}

		blanks++;
	}

exit:
	return blanks;
}

unsigned int
shape_gap_columns(const struct shape *shape)
{
	assert(shape);

	unsigned int blanks = 0;

	for (int c = 0; c < 4; ++c) {
		for (int r = 0; r < 4; ++r) {
			if (shape->def[0][r][c])
				goto exit;
		}

		blanks++;
	}

exit:
	return blanks;
}

void
shape_shuffle(struct shape *bag, size_t bagsz, enum shape_rand r)
{
	assert(bagsz >= r);

	size_t i, j;
	struct shape tmp;

	memset(bag, 0, sizeof (*bag) * bagsz);

	// First, we create an ordered sequence of every standard pieces in the
	// bag.
	for (i = 0; i < SHAPE_RAND_STANDARD; ++i) {
		memcpy(bag[i].def, shapes[i], sizeof (shapes[i]));
		bag[i].k = i;
	}

	// Shuffle the initial sequence.
	for (size_t p = 0; p < SHAPE_RAND_STANDARD - p; p++) {
		j = p + rand() / (RAND_MAX / (SHAPE_RAND_STANDARD - p) + 1);
		tmp = bag[j];
		bag[j] = bag[p];
		bag[p] = tmp;
	}

	// Now if the bag is bigger, add some random shapes as well. If we're
	// being asked to put some !standard pieces, do it less often because
	// they are very hard to positionate.
	for (; i < bagsz; ++i) {
		if (nrand(0, 3) == 0)
			bag[i].k = nrand(0, r - 1);
		else
			bag[i].k = nrand(0, SHAPE_RAND_STANDARD);

		memcpy(bag[i].def, shapes[bag[i].k], sizeof (bag[i].def));
	}
}

void
shape_rotate(struct shape *s, int o)
{
	assert(s);

	if (s->o + o >= 4)
		s->o = 0;
	else if (s->o + o < 0)
		s->o = 3;
	else
		s->o += o;
}

void
shape_move(struct shape *s, int dx, int dy)
{
	assert(s);

	s->x += dx;
	s->y += dy;
}
