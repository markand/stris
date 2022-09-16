/*
 * state.h -- game state
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

#ifndef STRIS_STATE_H
#define STRIS_STATE_H

enum key;

struct state {
	// Only once at startup.
	void (*init)(void);

	// When leaving the state.
	void (*suspend)(void);

	// When (re-)entering the state.
	void (*resume)(void);

	// On mouse motion.
	void (*onmouse)(int, int);

	// On mouse click.
	void (*onclick)(int, int);

	// On keypress.
	void (*onkey)(enum key);

	// Update the state logic.
	void (*update)(int);

	// Draw the state.
	void (*draw)(void);

	// Only once when quitting the application.
	void (*finish)(void);
};

void
state_init(struct state *);

void
state_suspend(struct state *);

void
state_resume(struct state *);

void
state_onmouse(struct state *, int, int);

void
state_onclick(struct state *, int, int);

void
state_onkey(struct state *, enum key);

void
state_update(struct state *, int);

void
state_draw(struct state *);

void
state_finish(struct state *);

#endif /* !STRIS_STATE_H */
