/*
 * score.c -- score handling
 *
 * Copyright (c) 2011, 2012 David Demelier <markand@malikania.fr>
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
#include <stdio.h>
#include <string.h>

#include "score.h"

void
score_read(struct score_list *list, const char *path)
{
	assert(list);
	assert(path);

	struct score *sc;
	FILE *fp;
	char fmt[32];

	memset(list, 0, sizeof (*list));
	snprintf(fmt, sizeof (fmt), "%%%d[^\n] %%d", SCORE_NAME_MAX);

	if (!(fp = fopen(path, "r")))
		return;

	while (list->scoresz < SCORE_LIST_MAX) {
		sc = &list->scores[list->scoresz];

		if (fscanf(fp, fmt, &sc->lines, sc->who) != 2)
			break;

		list->scoresz++;
	}
}

#if 0
int
score_check_top10(const score_list_t *list, const struct score *sc)
{
	struct score *s;

	if (TAILQ_EMPTY(list) || score_count(list) < 10)
		return true;

	TAILQ_FOREACH(s, list, link) {
		if (sc->score > s->score)
			return true;
	}

	return false;
}
#endif

void
score_add(struct score_list *list, const struct score *sc)
{
	(void)list;
	(void)sc;
#if 0
	struct score *s;
	struct score *news;

	if (!score_check_top10(list, sc))
		return;

	/* Alloc a new one, caller can safely pass a local variable */
	news = xmalloc(sizeof (*news));
	memcpy(news, sc, sizeof (*news));

	if (TAILQ_EMPTY(list))
		TAILQ_INSERT_HEAD(list, news, link);
	else {
		TAILQ_FOREACH(s, list, link)
			if (sc->score > s->score)
				break;

		if (!s)
			TAILQ_INSERT_TAIL(list, news, link);
		else
			TAILQ_INSERT_BEFORE(s, news, link);
	}

	/* Remove old bits > 10 */
	while (score_count(list) > 10) {
		s = TAILQ_LAST(list, score_list_t);
		TAILQ_REMOVE(list, s, link);
		free(s);
	}
#endif
}

void
score_write(const struct score_list *list, const char *path)
{
	FILE *fp;

	if (!(fp = fopen(path, "w")))
		return;

	for (size_t i = 0; i < list->scoresz; ++i)
		fprintf(fp, "%s %d\n", list->scores[i].who, list->scores[i].lines);

	fclose(fp);
}
