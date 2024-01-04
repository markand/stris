/*
 * board.h -- game main grid
 *
 * Copyright (c) 2011-2024 David Demelier <markand@malikania.fr>
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

#ifndef STRIS_BOARD_H
#define STRIS_BOARD_H

#define BOARD_W         (10)
#define BOARD_H         (20)

struct shape;

typedef int Board[BOARD_H][BOARD_W];

void
board_clear(Board);

void
board_pop(Board, int);

int
board_check(const Board, const struct shape *);

void
board_set(Board, const struct shape *);

void
board_unset(Board, const struct shape *);

#endif /* STRIS_BOARD_H */
