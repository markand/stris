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

/**
 * \file stris.h
 * \brief Main stris file.
 */

struct node;
struct coroutine;

/**
 * \enum mode
 * \brief Gameplay mode
 */
enum mode {
	MODE_STANDARD,          /*!< standard mode */
	MODE_EXTENDED,          /*!< extra pieces added */
	MODE_NIGHTMARE,         /*!< annoying pieces and messy board startup */
	MODE_LAST               /*!< number of modes */
};

/**
 * \enum key
 * \brief Key usage.
 */
enum key : unsigned int {
	KEY_NONE,               /*!< no keys */
	KEY_UP     = 1 << 0,    /*!< up and rotate */
	KEY_RIGHT  = 1 << 1,    /*!< right */
	KEY_DOWN   = 1 << 2,    /*!< down */
	KEY_LEFT   = 1 << 3,    /*!< left */
	KEY_CANCEL = 1 << 4,    /*!< escape or pause */
	KEY_SELECT = 1 << 5,    /*!< select menu entry */
	KEY_DROP   = 1 << 6     /*!< drop shape */
};

/**
 * \struct stris
 * \brief Game object and scene rendering.
 */
struct stris {
	/**
	 * Objects to draw on scene.
	 */
	struct node *nodes[32];

	/**
	 * Coroutines to manage.
	 */
	struct coroutine *coroutines[16];

	/**
	 * Keys being pressed.
	 */
	enum key keys;

	/**
	 * Non-zero as long as application should run.
	 */
	int run;
};

/**
 * \struct sconf
 * \brief STris configuration.
 */
struct sconf {
	int sound;              /*!< enable audio */
	int psychedelic;        /*!< enable background psychedelic effect */
	int scale;              /*!< increase window scaling */
};

/**
 * Global game state.
 */
extern struct stris stris;

/**
 * Global game configuration.
 */
extern struct sconf sconf;

/**
 * Yield until a new key is pressed.
 */
enum key
stris_pressed(void);

/**
 * Request to quit.
 */
void
stris_quit(void);

#endif /* STRIS_H */
