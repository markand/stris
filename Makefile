#
# Makefile -- mostly POSIX Makefile for stris
#
# Copyright (c) 2011-2022 David Demelier <markand@malikania.fr>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

.POSIX:

CC=                     cc
CFLAGS=                 -O3 -DNDEBUG

SDL2_INCS=              `pkg-config --cflags sdl2`
SDL2_LIBS=              `pkg-config --libs sdl2`

SDL2_IMAGE_INCS=        `pkg-config --cflags SDL2_image`
SDL2_IMAGE_LIBS=        `pkg-config --libs SDL2_image`

SDL2_TTF_INCS=          `pkg-config --cflags SDL2_ttf`
SDL2_TTF_LIBS=          `pkg-config --libs SDL2_ttf`

include config.mk

PROG=                   src/stris
SRCS=                   src/board.c \
                        src/menuitem.c \
                        src/score.c \
                        src/shape.c \
                        src/state.c \
                        src/state-menu.c \
                        src/state-splash.c \
                        src/tex.c \
                        src/ui.c \
                        src/util.c
OBJS=                   ${SRCS:.c=.o}
DEPS=                   ${SRCS:.c=.d}

DATA=                   data/fonts/actionj.h \
                        data/fonts/cartoon-relief.h \
                        data/fonts/dejavu-sans.h \
                        data/fonts/go-boom.h \
                        data/fonts/typography-ties.h \
                        data/img/blue.h \
                        data/img/cyan.h \
                        data/img/green.h \
                        data/img/orange.h \
                        data/img/panel.h \
                        data/img/purple.h \
                        data/img/red.h \
                        data/img/yellow.h

TESTS=                  tests/test-board.c
TESTS_OBJS=             ${TESTS:.c=}

INCS=                   -Idata \
                        -Iextern/libdt \
                        -Isrc \
                        ${SDL2_INCS} \
                        ${SDL2_IMAGE_INCS} \
                        ${SDL2_TTF_INCS}
LIBS=                   ${SDL2_LIBS} ${SDL2_IMAGE_LIBS} ${SDL2_TTF_LIBS}

.SUFFIXES:
.SUFFIXES: .h .o .c .png .ttf

all: ${PROG}

.c:
	${CC} ${INCS} ${CFLAGS} -o $@ $< ${OBJS} ${LIBS} ${LDFLAGS}

.c.o:
	${CC} ${INCS} ${NLS_CFLAGS} ${CFLAGS} -MMD -c $< -o $@

.ttf.h .png.h:
	extern/bcc/bcc -sc0 $< $< > $@

-include ${DEPS}

extern/bcc/bcc: extern/bcc/bcc.c
	${CC} ${CFLAGS} -o $@ extern/bcc/bcc.c ${LDFLAGS}

${DATA}: extern/bcc/bcc

${OBJS}: ${DATA}

${PROG}: ${OBJS}

clean:
	rm -f ${PROG} ${OBJS} ${DEPS} ${DATA} ${TESTS_OBJS}

${TESTS_OBJS}: ${OBJS}

tests: ${TESTS_OBJS}
	for t in ${TESTS_OBJS}; do ./$$t; done

.PHONY: all clean tests
