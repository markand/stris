/*
 * sound.h -- sound support and registry
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

#ifndef STRIS_SOUND_H
#define STRIS_SOUND_H

/**
 * \file sound.h
 * \brief Sound support and registry.
 */

/**
 * \enum sound
 * \brief Sound identifier
 */
enum sound {
	SOUND_CHIME,    /*!< splash screen */
	SOUND_MOVE,     /*!< shape rotation */
	SOUND_DROP,     /*!< Full drop */
	SOUND_CLEAN,    /*!< Line cleanup */
	SOUND_TICK      /*!< Short tick for death animation */
};

/**
 * Initialize the sound system.
 */
void
sound_init(void);

/**
 * Play the given sound.
 *
 * \param sound the sound to play
 */
void
sound_play(enum sound sound);

/**
 * Cleanup
 */
void
sound_finish(void);

#endif /* !STRIS_SOUND_H */
