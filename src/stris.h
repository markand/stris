/*
 * stris.h -- main stris file
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

#ifndef STRIS_H
#define STRIS_H

#include <stdio.h>

#if defined(WITH_NLS)
#       include <libintl.h>
#       define _(str)   gettext(str)
#else
#       define _(str)   str
#endif

#define STRIS_WIDTH     (350)
#define STRIS_HEIGHT    (450)

struct stris {
	int audio;
	int fullscreen;

	char path_conf[FILENAME_MAX + 1];
	char path_score[FILENAME_MAX + 1];
	char path_data[FILENAME_MAX + 1];
	char path_nls[FILENAME_MAX + 1];

	struct state *state;
	struct state *state_next;
};

extern struct stris stris;

void
stris_switch(struct state *);

void
stris_quit(void);

#endif /* STRIS_H */
