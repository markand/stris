/*
 * board.h -- game main grid
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

#ifndef STRIS_BOARD_H
#define STRIS_BOARD_H

/**
 * \file board.h
 * \brief Game main grid.
 */

/**
 * Number of columns in the board.
 */
#define BOARD_W 10

/**
 * Number of rows in the board.
 */
#define BOARD_H 20

struct shape;

/**
 * Typedef the board into two-dimensional array.
 */
typedef int Board[BOARD_H][BOARD_W];

/**
 * Clear the board.
 */
void
board_clear(Board board);

/**
 * Remove a line in the board, moving every other lines by one.
 *
 * \param line the line row
 */
void
board_pop(Board board, int line);

/**
 * Check if the shape can be placed at its position/orientation.
 *
 * \param shape the shape to check
 * \return non-zero if does not collide
 */
int
board_check(const Board board, const struct shape *shape);

/**
 * Place the shape in the board.
 *
 * \param shape the shape to place
 */
void
board_set(Board board, const struct shape *shape);

/**
 * Remove the shape from the board by emptying the cells.
 *
 * \param shape the shape to remove
 */
void
board_unset(Board board, const struct shape *shape);

#endif /* STRIS_BOARD_H */
