/*
 * util.h -- utilities
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

#ifndef STRIS_UTIL_H
#define STRIS_UTIL_H

/**
 * \file util.h
 * \brief Utilities.
 */

#include <stddef.h>

#include "port.h"

/**
 * Expand to array size.
 */
#define LEN(x) (sizeof (x) / sizeof (x[0]))

/**
 * Expand to parent struture from inner field.
 */
#define CONTAINER_OF(Ptr, Type, Field) \
	(Type *)((char *)(1 ? (Ptr) : &((Type *)0)->Field) - offsetof(Type, Field))

/**
 * Terminate program on error.
 *
 * Format as printf(3).
 */
PORT_PRINTF(1, 2)
void
die(const char *fmt, ...);

/**
 * Clamp value `v` in range `min..max`.
 */
long long int
clamp(long long int v, long long int min, long long int max);

/**
 * Get user account name.
 */
const char *
username(void);

/**
 * Generate random number in range min..max (max excluded).
 */
int
nrand(int min, int max);

/**
 * Allocate `n * w` bytes or exit with code 1.
 *
 * Bytes are zero'ed.
 */
void *
alloc(size_t n, size_t w);

/**
 * Duplicate pointer `ptr` of `w` number of bytes.
 */
void *
allocdup(const void *ptr, size_t w);

#endif /* !STRIS_UTIL_H */
