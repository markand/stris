/*
 * shape.c -- all tetris shapes
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

#include "shape.h"
#include "util.h"

static const int shapes[][4][4][4] = {
	// {{{ Standard
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
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 }
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
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 1 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
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
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 0 },
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
			{ 0, 1, 1, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
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
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
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
			{ 0, 1, 1, 1 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 0 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 0, 1 },
			{ 0, 0, 1, 1 },
			{ 0, 0, 0, 1 },
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

void
shape_select(struct shape *s, enum shape_rand r)
{
	assert(s);

	memset(s, 0, sizeof (*s));

	// Non-standard pieces are a bit harder to play so we don't spawn them
	// with the same chance as standard pieces.
	if (NRAND(0, 3) == 0)
		s->k = NRAND(1, r);
	else
		s->k = NRAND(1, SHAPE_RAND_STANDARD);

	memcpy(s->def, shapes[s->k], sizeof (s->def));
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
