/*
 * score.c -- score handling
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "score.h"
#include "util.h"

#define SCANF_FMT       ("%" STR(SCORE_NAME_MAX) "[^|]|%llu\n")
#define PRINTF_FMT      ("%s|%llu\n")

/*
 * Read the file properly formatted up to SCORE_COUNT_MAX.
 */

typedef int (*cmp_fn)(const void *, const void *);

static int
cmp(const struct score *s1, const struct score *s2)
{
	/* Can't use s2->score - s1->score as they are unsigned. */
	return s1->score < s2->score ? -1 : s2->score > s1->score ? 1 : 0;
}

static void
sort(struct score_list *list)
{
	assert(list);

	qsort(list->scores, list->scoresz, sizeof (list->scores[0]), (cmp_fn)cmp);
}

int
score_list_read(struct score_list *list, const char *path)
{
	assert(list);
	assert(path);

	FILE *fp;
	struct score s = {0};

	memset(list, 0, sizeof (*list));

	if (!(fp = fopen(path, "r")))
		return -1;

	for (size_t i = 0; i < SCORE_COUNT_MAX && fscanf(fp, SCANF_FMT, s.who, &s.score) >= 2; ++i) {
		memcpy(&list->scores[list->scoresz++], &s, sizeof (s));
		memset(&s, 0, sizeof (s));
	}

	fclose(fp);
	sort(list);

	return 0;
}

/*
 * Check if the score may be added into the high scores file.
 */
int
score_list_is_top(const struct score_list *list, const struct score *sc)
{
	assert(list);
	assert(sc);

	struct score *s;

	if (list->scoresz < SCORE_COUNT_MAX)
		return 1;

	for (size_t i = 0; i < list->scoresz; ++i)
		if (sc->score > list->scores[i].score)
			return 1;

	return 0;
}

void
score_list_add(struct score_list *list, const struct score *sc)
{
	size_t pos;

	for (pos = 0; pos < SCORE_COUNT_MAX; ++pos)
		if (sc->score > list->scores[pos].score)
			break;

	if (pos >= SCORE_COUNT_MAX)
		return;

	// [10] [9] [7] [5] [.]
	//          pos
	// [10] [9] [@] [7] [5]

	memmove(&list->scores[pos + 1], &list->scores[pos],
	    sizeof (*sc) * (SCORE_COUNT_MAX) - (&list->scores[pos] - list->scores));
	memcpy(&list->scores[pos], sc, sizeof (*sc));
}

/*
 * Write the new score file to the file specified by path.
 */
void
score_list_write(const struct score_list *list, const char *path)
{
	struct score *s;
	FILE *fp;

	if (!(fp = fopen(path, "w")))
		return;

	for (size_t i = 0; i < list->scoresz; ++i)
		fprintf(fp, PRINTF_FMT, list->scores[i].who, list->scores[i].score);

	fclose(fp);
}
