/*
 * sys.c -- operating system dependant routines
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

#if !defined(_WIN32)
#       include <sys/stat.h>
#       include <errno.h>
#else
#       include <windows.h>
#       include <fileapi.h>

#       if !defined(PATH_MAX)
#               define PATH_MAX MAX_PATH
#       endif
#endif

#include <limits.h>
#include <stdio.h>

#include <SDL.h>

#include "stris.h"
#include "sys.h"
#include "util.h"

#define ORG "fr.malikania"
#define APP "STris"

#if !defined(_WIN32)

static int
mkdirectory(const char *path)
{
	static const int mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

	if (mkdir(path, mode) < 0 && errno != EEXIST)
		return -1;

	return 0;
}

#else

static int
mkdirectory(const char *path)
{
	if (!CreateDirectoryA(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return -1;

	return 0;
}

#endif

static int
mkpath(char *path)
{
	char *p;

	for (p = path + 1; *p; ++p) {
		if (*p == '/') {
			*p = 0;

			if (mkdirectory(path) < 0)
				return -1;

			*p = '/';
		}
	}

	return mkdirectory(path);
}

static char *
path(void)
{
	static char ret[PATH_MAX];
	char *base;

	if (!(base = SDL_GetPrefPath(ORG, APP)))
		return NULL;

	// Silently create the directory now, it will fail anyway when trying
	// to write.
	if (mkpath(base) < 0)
		fprintf(stderr, "warning: unable to create %s\n", base);

	snprintf(ret, sizeof (ret), "%sstris.conf", base);
	SDL_free(base);

	return ret;
}

void
sys_conf_read(void)
{
	const char *p;
	FILE *fp;

	if (!(p = path()))
		return;
	if (!(fp = fopen(p, "r")))
		return;

	fscanf(fp, "%d %d\n", &sconf.sound, &sconf.scale);
	fclose(fp);

	// Reset to normal values if invalid.
	sconf.scale = clamp(sconf.scale, 1, 2);
}

void
sys_conf_write(void)
{
	char *p;
	FILE *fp;

	if (!(p = path()))
		return;
	if (!(fp = fopen(p, "w")))
		return;

	fprintf(fp, "%d %d\n", sconf.sound, sconf.scale);
	fclose(fp);
}
