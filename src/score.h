/*
 * score.h -- score handling
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

#ifndef STRIS_SCORE_H
#define STRIS_SCORE_H

#include <stddef.h>

#define SCORE_NAME_MAX 32
#define SCORE_LIST_MAX 10

struct score {
	char who[SCORE_NAME_MAX + 1];
	int lines;
};

struct score_list {
	struct score scores[SCORE_LIST_MAX];
	size_t scoresz;
};

void
score_read(struct score_list *, const char *);

void
score_add(struct score_list *, const struct score *);

void
score_write(const struct score_list *, const char *);

#endif /* STRIS_SCORE_H */
