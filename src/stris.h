/*
 * stris.h -- main stris file
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

#ifndef STRIS_H
#define STRIS_H

struct node;
struct coroutine;

enum mode : int {
	MODE_STANDARD,
	MODE_EXTENDED,
	MODE_NIGHTMARE,
	MODE_LAST
};

enum key : unsigned int {
	KEY_NONE,
	KEY_UP     = 1 << 0,
	KEY_RIGHT  = 1 << 1,
	KEY_DOWN   = 1 << 2,
	KEY_LEFT   = 1 << 3,
	KEY_CANCEL = 1 << 4,
	KEY_SELECT = 1 << 5,
	KEY_DROP   = 1 << 6,
};

extern struct stris {
	/*
	 * Object to draw on scene.
	 */
	struct node *nodes[32];

	struct coroutine *coroutines[16];

	/*
	 * Keys being pressed.
	 */
	enum key keys;
	int run;
} stris;

extern struct sconf {
	int sound;
	int psychedelic;
	int scale;
} sconf;

enum key
stris_pressed(void);

void
stris_quit(void);

#endif /* STRIS_H */
