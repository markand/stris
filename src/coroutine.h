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

struct mco_coro;

struct coroutine {
	void (*entry)(struct coroutine *self);
	void (*terminate)(struct coroutine *self);
	struct mco_coro *handle;
	unsigned int delay_acc;
	unsigned int delay_for;
};

void
coroutine_init(struct coroutine *co);

void
coroutine_sleep(unsigned int ms);

int
coroutine_resume(struct coroutine *co, unsigned int dt);

void
coroutine_yield(void);

void
coroutine_finish(struct coroutine *co);

#endif /* !STRIS_CORO_H */
