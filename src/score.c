/*
 * score.c -- score handling
 *
 * Copyright (c) 2011-2025 David Demelier <markand@malikania.fr>
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
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "score.h"
#include "stris.h"

//
// Depending on the system, we don't store the score files in the same
// directory and in the same permissions.
//
// Windows: C:\ProgramData\stris
// macOS:   In the Resources folder on top of the executable directory
// *:       $PREFIX/var/db/stris
//

#if !defined(PATH_MAX)
#       if defined(_POSIX_PATH_MAX)
#               define PATH_MAX _POSIX_PATH_MAX
#       else
#               define PATH_MAX 1024
#       endif
#endif

#if defined(__APPLE__)

static const char *
basedir(void)
{
	static char path[PATH_MAX];
	char *base;

	if ((base = SDL_GetBasePath())) {
		snprintf(path, sizeof (path), "%sdb", base);
		SDL_free(base);
	}

	return path;
}

#elif defined(_WIN32)

#include <shlobj.h>

static const char *
basedir(void)
{
	static char path[MAX_PATH];
	char base[MAX_PATH];
	int rv;

	rv = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, base);

	if (rv != S_OK)
		strcpy(path, ".");
	else {
		snprintf(path, sizeof (path), "%s\\stris", base);

		if (!CreateDirectoryA(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
			strcpy(path, ".");
	}

	return path;
}

#else

static const char *
basedir(void)
{
	static char path[PATH_MAX];

	snprintf(path, sizeof (path), VARDIR "/db/stris");

	return path;
}

#endif

const char *
score_path(enum stris_mode mode)
{
	static const char * const filenames[] = {
		"scores-s",
		"scores-e",
		"scores-n"
	};

	static char path[PATH_MAX];

	snprintf(path, sizeof (path), "%s/%s", basedir(), filenames[mode]);

	return path;
}

void
score_read(struct score_list *list, const char *path)
{
	assert(list);
	assert(path);

	struct score *sc;
	FILE *fp;
	char fmt[32];

	memset(list, 0, sizeof (*list));
	snprintf(fmt, sizeof (fmt), "%%%d[^:]:%%d\n", SCORE_NAME_MAX);

	if (!(fp = fopen(path, "r"))) {
		perror(path);
		return;
	}

	while (list->scoresz < SCORE_LIST_MAX) {
		sc = &list->scores[list->scoresz];

		if (fscanf(fp, fmt, sc->who, &sc->lines) != 2)
			break;

		list->scoresz++;
	}
}

void
score_add(struct score_list *list, const struct score *sc)
{
	size_t pos;

	for (pos = 0; pos < list->scoresz; ++pos)
		if (sc->lines >= list->scores[pos].lines)
			break;

	// Out of limits.
	if (pos >= SCORE_LIST_MAX)
		return;

	// Need to move existing scores?
	if (pos < list->scoresz)
		memmove(&list->scores[pos + 1], &list->scores[pos],
		    (list->scoresz - pos - 1) * sizeof (*sc));

	memcpy(&list->scores[pos], sc, sizeof (*sc));

	if (list->scoresz < SCORE_LIST_MAX)
		list->scoresz++;
}

void
score_write(const struct score_list *list, const char *path)
{
	FILE *fp;

	if (!(fp = fopen(path, "w"))) {
		perror(path);
		return;
	}

	for (size_t i = 0; i < list->scoresz; ++i)
		fprintf(fp, "%s:%d\n", list->scores[i].who, list->scores[i].lines);

	fclose(fp);
}
