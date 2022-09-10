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
state_handle(struct state *s, const SDL_Event *ev)
{
	assert(s);
	assert(ev);

	if (s->handle)
		s->handle(ev);
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
