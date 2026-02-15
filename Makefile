#
# Makefile -- Makefile for STris
#
# Copyright (c) 2011-2023 David Demelier <markand@malikania.fr>
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

-include config.mk

CC ?= clang

# The pkg-config(1) utility.
PKGCONF ?= pkg-config

# Compiler option to generate .d files.
MD ?= -MMD

# Installation paths.
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
VARDIR ?= $(PREFIX)/var
MANDIR ?= $(PREFIX)/share/man

# Path to libraries.
MATH_LIBS ?= -lm

SDL3_INCS ?= $(shell $(PKGCONF) --cflags sdl3)
SDL3_LIBS ?= $(shell $(PKGCONF) --libs sdl3)

SDL3_IMAGE_INCS ?= $(shell $(PKGCONF) --cflags sdl3-image)
SDL3_IMAGE_LIBS ?= $(shell $(PKGCONF) --libs sdl3-image)

SDL3_MIXER_INCS ?= $(shell $(PKGCONF) --cflags sdl3-mixer)
SDL3_MIXER_LIBS ?= $(shell $(PKGCONF) --libs sdl3-mixer)

SDL3_TTF_INCS ?= $(shell $(PKGCONF) --cflags sdl3-ttf)
SDL3_TTF_LIBS ?= $(shell $(PKGCONF) --libs sdl3-ttf)

# No user modifications below this line.

VERSION = 0.5.0

PROG = src/stris

SRCS += src/board.c
SRCS += src/coroutine.c
SRCS += src/joy.c
SRCS += src/list.c
SRCS += src/node.c
SRCS += src/score.c
SRCS += src/shape.c
SRCS += src/sound.c
SRCS += src/state-menu.c
SRCS += src/state-mode.c
SRCS += src/state-play.c
SRCS += src/state-scores.c
SRCS += src/state-settings.c
SRCS += src/state-splash.c
SRCS += src/stris.c
SRCS += src/sys.c
SRCS += src/texture.c
SRCS += src/ui.c
SRCS += src/util.c

ASSETS += assets/fonts/actionj.h
ASSETS += assets/fonts/cartoon-relief.h
ASSETS += assets/fonts/instruction.h
ASSETS += assets/fonts/typography-ties.h
ASSETS += assets/gamecontrollerdb.h
ASSETS += assets/img/block1.h
ASSETS += assets/img/block10.h
ASSETS += assets/img/block11.h
ASSETS += assets/img/block12.h
ASSETS += assets/img/block2.h
ASSETS += assets/img/block3.h
ASSETS += assets/img/block4.h
ASSETS += assets/img/block5.h
ASSETS += assets/img/block6.h
ASSETS += assets/img/block7.h
ASSETS += assets/img/block8.h
ASSETS += assets/img/block9.h
ASSETS += assets/sound/clean.h
ASSETS += assets/sound/drop.h
ASSETS += assets/sound/move.h
ASSETS += assets/sound/startup.h
ASSETS += assets/sound/tick.h

OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

GCDB := https://raw.githubusercontent.com/mdqinc/SDL_GameControllerDB/refs/heads/master/gamecontrollerdb.txt

override CFLAGS += $(SDL3_IMAGE_INCS)
override CFLAGS += $(SDL3_INCS)
override CFLAGS += $(SDL3_MIXER_INCS)
override CFLAGS += $(SDL3_TTF_INCS)
override CFLAGS += $(MD)
override CFLAGS += -DVARDIR=\"$(VARDIR)\"
override CFLAGS += -Iassets
override CFLAGS += -Iextern/minicoro
override CFLAGS += -Isrc

override LDLIBS += $(MATH_LIBS)
override LDLIBS += $(SDL3_IMAGE_LIBS)
override LDLIBS += $(SDL3_LIBS)
override LDLIBS += $(SDL3_MIXER_LIBS)
override LDLIBS += $(SDL3_TTF_LIBS)

CMD.cc ?= $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
CMD.link ?= $(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)
CMD.bcc ?= extern/bcc/bcc -sc0 $< $< > $@

.PHONY: all
all: $(PROG)

%: %.o
	$(CMD.link)
%.exe: %.o
	$(CMD.link)
%.o: %.c
	$(CMD.cc)

%.h: %.otf
	$(CMD.bcc)
%.h: %.png
	$(CMD.bcc)
%.h: %.ttf
	$(CMD.bcc)
%.h: %.txt
	$(CMD.bcc)
%.h: %.wav
	$(CMD.bcc)

-include $(DEPS)

$(ASSETS): | extern/bcc/bcc

$(SRCS): $(ASSETS)
$(PROG): $(OBJS)

.PHONY: install
install:
	mkdir -p $(DESTDIR)$(BINDIR)
	cp $(PROG) $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(VARDIR)/db/stris
	chmod 775 $(DESTDIR)$(VARDIR)/db/stris
	mkdir -p $(DESTDIR)$(MANDIR)/man6
	sed -e "s,@VARDIR@,$(VARDIR),g" < src/stris.6 > $(DESTDIR)$(MANDIR)/man6/stris.6

.PHONY: macos-app
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

.PHONY: mingw-app
mingw-app:
	rm -rf STris-$(VERSION)
	mkdir STris-$(VERSION)
	cp stris.exe STris-$(VERSION)/STris.exe
	./win/mingw-bundledlls --copy STris-$(VERSION)/STris.exe

.PHONY: clean
clean:
	rm -f extern/bcc/bcc extern/bcc/bcc.d
	rm -f $(PROG) $(OBJS) $(DEPS) $(ASSETS)
	rm -rf STris-$(VERSION) STris.app

.PHONY: update-gcdb
update-gcdb:
	@echo "Updating gamecontrollerdb.txt..."
	@wget -nv -O data/gamecontrollerdb.txt $(GCDB)

.PHONY: .clangd
.clangd:
	echo "CompileFlags:" > $@
	echo "  Add:" >> $@
	echo "    - -DVERSION=\"$(VERSION)\"" >> $@
	echo "    - -I$(CURDIR)/data" >> $@
	echo "    - -I$(CURDIR)/src" >> $@
	echo $(SDL2_INCS) $(SDL2_IMAGE_INCS) $(SDL2_MIXER_INCS) $(SDL2_TTF_INCS) \
	    | sed 's, ,\n,g' | grep -Ev '^$$' | sed 's,^,    - ,g' >> $@

# Convenient target to regenerate compile_commands.json.
.PHONY: compile_commands.json
compile_commands.json:
	@$(MAKE) --always-make --dry-run all \
		| grep -wE -- "$(CC)" \
		| jq -nR '[inputs|{directory:"$(CURDIR)", command:., file: match("[^ ]+[.]c").string[0:]}]' \
		> $@
