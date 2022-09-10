/*
 * test-board.c -- test board routines
 *
 * Copyright (c) 2011-2021 David Demelier <markand@malikania.fr>
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

#include <dt.h>

#include "board.h"
#include "shape.h"

static void
test_board_clear(void)
{
	Board b;

	board_clear(b);

	for (int r = 0; r < BOARD_H; ++r)
		for (int c = 0; c < BOARD_W; ++c)
			DT_EQ_INT(b[r][c], 0);
}

#if 1

static inline void
show(const Board b)
{
	printf("      [0] [1] [2] [3] [4] [5] [6] [7] [8] [9]\n");

	for (int r = 0; r < BOARD_H; ++r) {
		printf("[%2d] = ", r);

		for (int c = 0; c < BOARD_W; ++c)
			printf("%d,  ", b[r][c]);

		printf("\n");
	}
}

#endif

static void
test_board_pop(void)
{
	Board b = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, /* (12) We remove this */
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
		{ 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 },
		{ 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	};

#if 0
	/* What is expected */
	Board b = {
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 0 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 1 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 2 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 3 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 4 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 5 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 6 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 7 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 8 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 9 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 10 */
		{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* 11 */
		{ 0, 0, 0, 0, 0, 1, 1, 0, 0, 0 }, /* 12 */
		{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 }, /* 13 */
		{ 0, 1, 1, 1, 1, 1, 1, 0, 0, 0 }, /* 14 */
		{ 0, 1, 1, 1, 1, 0, 0, 0, 0, 0 }, /* 15 */
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, /* 16 */
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, /* 17 */
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, /* 18 */
		{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, /* 19 */
	};
#endif
	board_pop(b, 12);

	/* 0-11 is 0. */
	for (int r = 0; r < 12; ++r)
		for (int c = 0; c < BOARD_W; ++c)
			DT_EQ_INT(b[r][c], 0);

	/* row 12 */
	DT_EQ_INT(b[12][0], 0);
	DT_EQ_INT(b[12][1], 0);
	DT_EQ_INT(b[12][2], 0);
	DT_EQ_INT(b[12][3], 0);
	DT_EQ_INT(b[12][4], 0);
	DT_EQ_INT(b[12][5], 1);
	DT_EQ_INT(b[12][6], 1);
	DT_EQ_INT(b[12][7], 0);
	DT_EQ_INT(b[12][8], 0);
	DT_EQ_INT(b[12][9], 0);

	/* row 13 */
	DT_EQ_INT(b[13][0], 0);
	DT_EQ_INT(b[13][1], 0);
	DT_EQ_INT(b[13][2], 0);
	DT_EQ_INT(b[13][3], 0);
	DT_EQ_INT(b[13][4], 0);
	DT_EQ_INT(b[13][5], 0);
	DT_EQ_INT(b[13][6], 0);
	DT_EQ_INT(b[13][7], 1);
	DT_EQ_INT(b[13][8], 0);
	DT_EQ_INT(b[13][9], 0);

	/* row 14 */
	DT_EQ_INT(b[14][0], 0);
	DT_EQ_INT(b[14][1], 1);
	DT_EQ_INT(b[14][2], 1);
	DT_EQ_INT(b[14][3], 1);
	DT_EQ_INT(b[14][4], 1);
	DT_EQ_INT(b[14][5], 1);
	DT_EQ_INT(b[14][6], 1);
	DT_EQ_INT(b[14][7], 0);
	DT_EQ_INT(b[14][8], 0);
	DT_EQ_INT(b[14][9], 0);

	/* row 15 */
	DT_EQ_INT(b[15][0], 0);
	DT_EQ_INT(b[15][1], 1);
	DT_EQ_INT(b[15][2], 1);
	DT_EQ_INT(b[15][3], 1);
	DT_EQ_INT(b[15][4], 1);
	DT_EQ_INT(b[15][5], 0);
	DT_EQ_INT(b[15][6], 0);
	DT_EQ_INT(b[15][7], 0);
	DT_EQ_INT(b[15][8], 0);
	DT_EQ_INT(b[15][9], 0);

	/* row 16 */
	DT_EQ_INT(b[16][0], 1);
	DT_EQ_INT(b[16][1], 1);
	DT_EQ_INT(b[16][2], 1);
	DT_EQ_INT(b[16][3], 1);
	DT_EQ_INT(b[16][4], 1);
	DT_EQ_INT(b[16][5], 1);
	DT_EQ_INT(b[16][6], 1);
	DT_EQ_INT(b[16][7], 1);
	DT_EQ_INT(b[16][8], 1);
	DT_EQ_INT(b[16][9], 1);

	/* row 17 */
	DT_EQ_INT(b[17][0], 1);
	DT_EQ_INT(b[17][1], 1);
	DT_EQ_INT(b[17][2], 1);
	DT_EQ_INT(b[17][3], 1);
	DT_EQ_INT(b[17][4], 1);
	DT_EQ_INT(b[17][5], 1);
	DT_EQ_INT(b[17][6], 1);
	DT_EQ_INT(b[17][7], 1);
	DT_EQ_INT(b[17][8], 1);
	DT_EQ_INT(b[17][9], 1);

	/* row 18 */
	DT_EQ_INT(b[18][0], 1);
	DT_EQ_INT(b[18][1], 1);
	DT_EQ_INT(b[18][2], 1);
	DT_EQ_INT(b[18][3], 1);
	DT_EQ_INT(b[18][4], 1);
	DT_EQ_INT(b[18][5], 1);
	DT_EQ_INT(b[18][6], 1);
	DT_EQ_INT(b[18][7], 1);
	DT_EQ_INT(b[18][8], 1);
	DT_EQ_INT(b[18][9], 1);

	/* row 19 */
	DT_EQ_INT(b[19][0], 1);
	DT_EQ_INT(b[19][1], 1);
	DT_EQ_INT(b[19][2], 1);
	DT_EQ_INT(b[19][3], 1);
	DT_EQ_INT(b[19][4], 1);
	DT_EQ_INT(b[19][5], 1);
	DT_EQ_INT(b[19][6], 1);
	DT_EQ_INT(b[19][7], 1);
	DT_EQ_INT(b[19][8], 1);
	DT_EQ_INT(b[19][9], 1);
}

static void
test_board_set(void)
{
	struct shape s;
	Board b;

	shape_select(&s, SHAPE_T);
	s.x = 2;
	s.y = 2;

	board_clear(b);
	board_set(b, &s);

	show(b);
}

int
main(int argc, char **argv)
{
	DT_RUN(test_board_clear);
	DT_RUN(test_board_pop);
	DT_RUN(test_board_set);
	DT_SUMMARY();

	return DT_EXIT();
}
