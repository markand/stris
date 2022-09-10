/*
 * sys.c -- system routines
 *
 * Copyright (c) 2020 David Demelier <markand@malikania.fr>
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

#include "sysconfig.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#if defined(_WIN32)
#       include <shlwapi.h>
#       include <windows.h>
#else
#       include <sys/stat.h>
#       include <errno.h>
#       include <string.h>
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "error.h"
#include "sys.h"

static struct {
	char organization[128];
	char name[128];
} info = {
	.organization = "fr.malikania",
	.name = "stris"
};

static const char *paths[] = {
	[SYS_DIR_BIN]           = MOLKO_BINDIR,
	[SYS_DIR_DATA]          = MOLKO_DATADIR,
	[SYS_DIR_LOCALE]        = MOLKO_LOCALEDIR
};

static const char *abspaths[] = {
	[SYS_DIR_BIN]           = MOLKO_ABS_BINDIR,
	[SYS_DIR_DATA]          = MOLKO_ABS_DATADIR,
	[SYS_DIR_LOCALE]        = MOLKO_ABS_LOCALEDIR
};

#if defined(_WIN32)

static bool
is_absolute(const char *path)
{
	return !PathIsRelativeA(path);
}

#else

static bool
is_absolute(const char *path)
{
	return path[0] == '/';
}

#endif

static char *
normalize(char *str)
{
	for (char *p = str; *p; ++p)
		if (*p == '\\')
			*p = '/';

	return str;
}

bool
sys_init(const char *organization, const char *name)
{
#if defined(__MINGW64__)
	/* On MinGW buffering leads to painful debugging. */
	setbuf(stderr, NULL);
	setbuf(stdout, NULL);
#endif

	strlcpy(info.organization, organization, sizeof (info.organization));
	strlcpy(info.name, name, sizeof (info.name));

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		return errorf("%s", SDL_GetError());
	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
		return errorf("%s", SDL_GetError());
	if (TTF_Init() < 0)
		return errorf("%s", SDL_GetError());
	if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG)
		return errorf("%s", SDL_GetError());
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
		return errorf("%s", SDL_GetError());

	Mix_AllocateChannels(SOUND_CHANNELS_MAX);

	return true;
}

static const char *
absolute(const char *which)
{
	static char path[PATH_MAX];

	strlcpy(path, which, sizeof (path));

	return normalize(path);
}

static const char *
system_directory(enum sys_dir kind)
{
	static char path[PATH_MAX];
	static char ret[PATH_MAX];
	char *base, *binsect;

	/* 1. Get current binary directory. */
	base = SDL_GetBasePath();

	/*
	 * 2. Decompose the path to the given special directory by computing
	 *    relative directory to it from where the binary is located.
	 *
	 * Example:
	 *   PREFIX/bin/mlk
	 *   PREFIX/share/molko
	 *
	 * The path to the data is ../share/molko starting from the binary.
	 *
	 * If path to binary is absolute we can't determine relative paths to
	 * any other directory and use the absolute one instead.
	 *
	 * Also, on some platforms SDL_GetBasePath isn't implemented and returns
	 * NULL, in that case return the fallback to the installation prefix.
	 */
	if (is_absolute(paths[SYS_DIR_BIN]) || is_absolute(paths[kind]) || !base)
		return absolute(abspaths[kind]);

	/*
	 * 3. Put the base path into the path and remove the value of
	 *    MOLKO_BINDIR.
	 *
	 * Example:
	 *   from: /usr/local/bin
	 *   to:   /usr/local
	 */
	strlcpy(path, base, sizeof (path));
	SDL_free(base);

	if ((binsect = strstr(path, paths[SYS_DIR_BIN])))
		*binsect = '\0';

	snprintf(ret, sizeof (ret), "%s%s", path, paths[kind]);

	return normalize(ret);
}

static const char *
user_directory(enum sys_dir kind)
{
	/* Kept for future use. */
	(void)kind;

	static char path[PATH_MAX];
	char *pref;

	if ((pref = SDL_GetPrefPath(info.organization, info.name))) {
		strlcpy(path, pref, sizeof (path));
		SDL_free(pref);
	} else
		strlcpy(path, "./", sizeof (path));

	return path;
}

static bool
mkpath(const char *path)
{
#ifdef _WIN32
	/* TODO: add error using the convenient FormatMessage function. */
	if (!CreateDirectoryA(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return errorf("unable to create directory: %s", path);
#else
	if (mkdir(path, 0755) < 0 && errno != EEXIST)
		return errorf("%s", strerror(errno));
#endif

	return true;
}

const char *
sys_dir(enum sys_dir kind)
{
	switch (kind) {
	case SYS_DIR_BIN:
	case SYS_DIR_DATA:
	case SYS_DIR_LOCALE:
		return system_directory(kind);
	default:
		return user_directory(kind);
	}
}

bool
sys_mkdir(const char *directory)
{
	char path[PATH_MAX], *p;

	/* Copy the directory to normalize and iterate over '/'. */
	strlcpy(path, directory, sizeof (path));
	normalize(path);

#if defined(_WIN32)
	/* Remove drive letter that we don't need. */
	if ((p = strchr(path, ':')))
		++p;
	else
		p = path;
#else
	p = path;
#endif

	for (p = p + 1; *p; ++p) {
		if (*p == '/') {
			*p = 0;

			if (!mkpath(path))
				return false;

			*p = '/';
		}
	}

	return mkpath(path);
}

void
sys_finish(void)
{
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
