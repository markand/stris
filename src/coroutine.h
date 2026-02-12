/*
 * coroutine.h -- coroutine support
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

#ifndef STRIS_CORO_H
#define STRIS_CORO_H

/**
 * \file coroutine.h
 * \brief Coroutine support.
 */

struct mco_coro;

/**
 * \struct coroutine
 * \brief Coroutine object.
 */
struct coroutine {
	/**
	 * (init)
	 *
	 * Coroutine entrypoint.
	 *
	 * When this function terminates the coroutine is removed from the
	 * window.
	 */
	void (*entry)(struct coroutine *self);

	/**
	 * (optional)
	 *
	 * Function to call when the coroutine is destroyed.
	 *
	 * The function can safely free the pointer owning the coroutine if it
	 * is dynamically allocated.
	 */
	void (*terminate)(struct coroutine *self);

	struct mco_coro *handle;        /* minicoro handle */
	unsigned int delay_acc;         /* coroutine_sleep accumulator */
	unsigned int delay_for;         /* coroutine_sleep yield */
};

/**
 * Initialize a coroutine and attach it to the game window.
 *
 * The coroutine will be resumed in the main game loop.
 */
void
coroutine_init(struct coroutine *co);

/**
 * Yield the calling coroutine for the given amount of milliseconds.
 *
 * \param ms the time to sleep in milliseconds
 */
void
coroutine_sleep(unsigned int ms);

/**
 * Resume the coroutine if one of the condition applies:
 *
 * - The coroutine is suspended (yielding).
 * - The coroutine is waiting (yielding in coroutine_sleep) and the total
 *   accumulated time has elapsed.
 *
 * \param dt time difference since last frame
 */
int
coroutine_resume(struct coroutine *co, unsigned int dt);

/**
 * Yield the calling coroutine.
 */
void
coroutine_yield(void);

/**
 * Remove coroutine from stris main loop and destroy it.
 */
void
coroutine_finish(struct coroutine *co);

#endif /* !STRIS_CORO_H */
