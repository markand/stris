/*
 * util.c -- utilities
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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For username()
#if !defined(_WIN32)
#       include <sys/types.h>
#       include <pwd.h>
#       include <unistd.h>
#else
#       include <lmcons.h>
#       include <windows.h>
#endif

// For nrand()
#if defined(__linux__)
#      include <sys/random.h>
#endif

#include "util.h"

void
die(const char *fmt, ...)
{
	assert(fmt);

	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

long long int
clamp(long long int v, long long int min, long long int max)
{
	return v < min ? min : v > max ? max : v;
}

#if !defined(_WIN32)

const char *
username(void)
{
	const struct passwd *pw;

	if (!(pw = getpwuid(getuid())))
		return "anon";

	return pw->pw_name;
}

#else

const char *
username(void)
{
	static char username[UNLEN + 1];
	DWORD size = sizeof (username);

	if (!GetUserNameA(username, &size))
		strcpy(username, "anon");

	return username;
}

#endif

int
nrand(int min, int max)
{
	unsigned int val;

#if defined(__linux__)
	getrandom(&val, sizeof (val), GRND_RANDOM);
#elif defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
	val = arc4random();
#else
	val = rand();
#endif

	return (val % (max - min + 1)) + min;
}

void *
alloc(size_t n, size_t w)
{
	void *ptr;

	if (!(ptr = calloc(n, w)))
		die("calloc: %s\n", strerror(ENOMEM));

	return ptr;
}

void *
allocdup(const void *src, size_t w)
{
	void *ptr;

	ptr = alloc(1, w);

	return memcpy(ptr, src, w);
}
