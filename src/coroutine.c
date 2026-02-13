/*
 * coroutine.c -- coroutine support
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
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "coroutine.h"
#include "stris.h"
#include "util.h"

/* minicoro configuration. */
#define MCO_ALLOC(w) alloc(1, w)
#define MCO_DEALLOC(p, w) free(p)
#define MCO_DEFAULT_STACK_SIZE 524288
#define MCO_MIN_STACK_SIZE MCO_DEFAULT_STACK_SIZE
#define MCO_NO_MULTITHREAD

#define MINICORO_IMPL
#include <minicoro.h>

static void
coroutine_entry(struct mco_coro *self)
{
	struct coroutine *co = self->user_data;

	co->entry(co);
}

static inline struct coroutine *
coroutine_self(void)
{
	return mco_running()->user_data;
}

void
coroutine_init(struct coroutine *co)
{
	assert(co);
	assert(co->entry);

	struct coroutine **slot = NULL;
	struct mco_desc desc;
	int rc;

	desc = mco_desc_init(coroutine_entry, 0);
	desc.user_data = co;

	if ((rc = mco_create(&co->handle, &desc)) != MCO_SUCCESS)
		die("mco_create: %d\n", rc);

	/* register in stris stage */
	for (size_t i = 0; i < LEN(stris.coroutines); ++i) {
		if (stris.coroutines[i] == NULL) {
			slot = &stris.coroutines[i];
			break;
		}
	}

	if (!slot)
		die("abort: coroutine space exceeded\n");

	*slot = co;
}

void
coroutine_sleep(unsigned int ms)
{
	assert(ms);

	struct coroutine *co;

	co = coroutine_self();
	co->delay_for = ms;
	co->delay_acc = 0;

	while (co->delay_acc < co->delay_for)
		coroutine_yield();
}

int
coroutine_resume(struct coroutine *co, unsigned int dt)
{
	assert(co);

	int rc, resume = 0;

	if (co->delay_for) {
		co->delay_acc += dt;

		if (co->delay_acc >= co->delay_for) {
			co->delay_for = co->delay_acc = 0;
			resume = 1;
		}
	} else
		resume = 1;

	if (resume && (rc = mco_resume(co->handle)) != MCO_SUCCESS)
		die("mco_resume: %d\n", rc);

	return mco_status(co->handle) == MCO_DEAD;
}

void
coroutine_yield(void)
{
	int rc;

	if ((rc = mco_yield(mco_running())) != MCO_SUCCESS)
		die("mco_yield: %d\n", rc);
}

void
coroutine_finish(struct coroutine *co)
{
	assert(co);

	int rc;

	if (!co->handle)
		return;

	if ((rc = mco_destroy(co->handle)) != MCO_SUCCESS)
		die("mco_destroy: %d\n", rc);

	/* Remove coroutine from stris. */
	for (size_t i = 0; i < LEN(stris.coroutines); ++i) {
		if (stris.coroutines[i] == co) {
			stris.coroutines[i] = NULL;
			break;
		}
	}

	co->handle = NULL;

	if (co->terminate)
		co->terminate(co);
}
