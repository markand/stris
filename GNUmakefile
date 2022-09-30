#
# GNUmakefile -- Makefile for STris
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

VERSION :=              0.5.0

# Host compiler.
HOST_CC :=              clang
HOST_CFLAGS :=          -O3 -DNDEBUG
HOST_LDFLAGS :=

# Target compiler.
CC :=                   clang
CFLAGS :=               -O3 -DNDEBUG
PKGCONF :=              pkg-config

# User and groups (mostly for scores).
UID :=                  root
GID :=                  games

# Installation paths.
PREFIX :=               /usr/local
BINDIR :=               $(PREFIX)/bin
VARDIR :=               $(PREFIX)/var

# Path to libraries.
SDL2_INCS :=            $(shell $(PKGCONF) --cflags sdl2)
SDL2_LIBS :=            $(shell $(PKGCONF) --libs sdl2)

SDL2_IMAGE_INCS :=      $(shell $(PKGCONF) --cflags SDL2_image)
SDL2_IMAGE_LIBS :=      $(shell $(PKGCONF) --libs SDL2_image)

SDL2_MIXER_INCS :=      $(shell $(PKGCONF) --cflags SDL2_mixer)
SDL2_MIXER_LIBS :=      $(shell $(PKGCONF) --libs SDL2_mixer)

SDL2_TTF_INCS :=        $(shell $(PKGCONF) --cflags SDL2_ttf)
SDL2_TTF_LIBS :=        $(shell $(PKGCONF) --libs SDL2_ttf)

# No user modifications below this line.

-include config.mk

PROG :=                 stris
SRCS :=                 src/board.c \
                        src/list.c \
                        src/score.c \
                        src/shape.c \
                        src/sound.c \
                        src/state-dead.c \
                        src/state-menu.c \
                        src/state-mode.c \
                        src/state-play.c \
                        src/state-scores.c \
                        src/state-settings.c \
                        src/state-splash.c \
                        src/state.c \
                        src/stris.c \
                        src/sys.c \
                        src/tex.c \
                        src/ui.c \
                        src/util.c
OBJS :=                 $(SRCS:.c=.o)
DEPS :=                 $(SRCS:.c=.d)

DATA :=                 data/fonts/actionj.h \
                        data/fonts/cartoon-relief.h \
                        data/fonts/instruction.h \
                        data/fonts/typography-ties.h \
                        data/img/block1.h \
                        data/img/block2.h \
                        data/img/block3.h \
                        data/img/block4.h \
                        data/img/block5.h \
                        data/img/block6.h \
                        data/img/block7.h \
                        data/img/block8.h \
                        data/img/block9.h \
                        data/img/block10.h \
                        data/img/block11.h \
                        data/img/block12.h \
                        data/sound/clean.h \
                        data/sound/drop.h \
                        data/sound/move.h \
                        data/sound/startup.h

DEFS :=                 -DVARDIR=\"$(VARDIR)\"
INCS :=                 -Idata \
                        -Isrc \
                        $(SDL2_INCS) \
                        $(SDL2_IMAGE_INCS) \
                        $(SDL2_TTF_INCS)
LIBS :=                 -lm \
                        $(SDL2_LIBS) \
                        $(SDL2_IMAGE_LIBS) \
                        $(SDL2_MIXER_LIBS) \
                        $(SDL2_TTF_LIBS)

.SUFFIXES:
.SUFFIXES: .h .o .c .otf .png .ttf .wav

all: $(PROG)

.c.o:
	$(CC) $(DEFS) $(INCS) $(CFLAGS) -MMD -c $< -o $@

.otf.h .png.h .ttf.h .wav.h:
	./extern/bcc/bcc -sc0 $< $< > $@

-include $(DEPS)

extern/bcc/bcc: extern/bcc/bcc.c
	$(HOST_CC) $(HOST_CFLAGS) -o $@ extern/bcc/bcc.c $(HOST_LDFLAGS)

$(DATA): extern/bcc/bcc

$(OBJS): $(DATA)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS)

install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(PROG) $(DESTDIR)$(BINDIR)
	chown $(UID):$(GID) $(DESTDIR)$(BINDIR)/$(PROG)
	chmod 2755 $(DESTDIR)$(BINDIR)/$(PROG)
	mkdir -p $(DESTDIR)$(VARDIR)/db/stris
	chown $(UID):$(GID) $(DESTDIR)$(VARDIR)/db/stris
	chmod 775 $(DESTDIR)$(VARDIR)/db/stris

macos-app:
	rm -rf STris.app
	mkdir -p STris.app
	mkdir -p STris.app/Contents/MacOS
	mkdir -p STris.app/Contents/Resources/db
	chmod 777 STris.app/Contents/Resources/db
	mkdir -p STris.app/Contents/Frameworks
	cp $(PROG) STris.app/Contents/MacOS/STris
	cp apple/Info.plist STris.app/Contents
	dylibbundler -od -of -b -ns \
		-d STris.app/Contents/Frameworks \
		-x STris.app/Contents/MacOS/STris \
		-p @executable_path/../Frameworks
	chgrp -R staff STris.app

win-app:
	rm -rf STris-$(VERSION)
	mkdir STris-$(VERSION)
	cp stris.exe STris-$(VERSION)/STris.exe
	./win/mingw-bundledlls --copy STris-$(VERSION)/STris.exe

clean:
	rm -f extern/bcc/bcc $(PROG) $(OBJS) $(DEPS) $(DATA)
	rm -rf STris-$(VERSION) STris.app

.PHONY: all clean tests
