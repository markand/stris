/*
 * state.c -- game state
 *
 * Copyright (c) 2011-2021 David Demelier <markand@malikania.fr>
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

#include "key.h"
#include "state.h"

void
state_start(struct state *s)
{
	assert(s);

	if (s->start)
		s->start();
}

void
state_suspend(struct state *s)
{
	assert(s);

	if (s->suspend)
		s->suspend();
}

void
state_resume(struct state *s)
{
	assert(s);

	if (s->resume)
		s->resume();
}

void
state_onmouse(struct state *s, int x, int y)
{
	assert(s);

	if (s->onmouse)
		s->onmouse(x, y);
}

void
state_onclick(struct state *s, int x, int y)
{
	assert(s);

	if (s->onclick)
		s->onclick(x, y);
}

void
state_onkey(struct state *s, enum key key)
{
	assert(s);

	if (s->onkey)
		s->onkey(key);
}

void
state_update(struct state *s, int ticks)
{
	assert(s);

	if (s->update)
		s->update(ticks);
}

void
state_draw(struct state *s)
{
	assert(s);

	if (s->draw)
		s->draw();
}

void
state_finish(struct state *s)
{
	assert(s);

	if (s->finish)
		s->finish();
}
