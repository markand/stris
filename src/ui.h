/*
 * ui.h -- user interface rendering
 *
 * Copyright (c) 2011-2022 David Demelier <markand@malikania.fr>
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

#ifndef STRIS_UI_H
#define STRIS_UI_H

#include <SDL.h>

#define UI_W 360
#define UI_H 640

extern SDL_Window *ui_win;
extern SDL_Renderer *ui_rdr;

extern const unsigned long ui_palette[16];

void
ui_init(void);

void
ui_set_color(unsigned long);

void
ui_clear(void);

void
ui_present(void);

void
ui_quit(void);

#endif /* !STRIS_UI_H */

#if 0

#ifndef _UI_H_
#define _UI_H_

#include <wing/font.h>
#include <wing/menu.h>

#include "sd-tris.h"
#include "game.h"
#include "array.h"

/* Shape colors */
enum {
	UiShapeColorYellow	= ShapeO,
	UiShapeColorCyan	= ShapeI,
	UiShapeColorGreen	= ShapeS,
	UiShapeColorRed		= ShapeZ,
	UiShapeColorOrange	= ShapeL,
	UiShapeColorBlue	= ShapeJ,
	UiShapeColorMagenta	= ShapeT,

	UiShapeColorLAST
};

/*
 * Fonts, fonts prefixed by (u) in the comment means that these
 * fonts may be updated during game.
 */
enum {
	UiFontTitle,			/* SD-Tris title */
	UiFontTitleShadow,		/* SD-Tris shadow */
	UiFontMenu,			/* nenu entry */
	UiFontMenuSelected,		/* (U) selected entry */
	UiFontPause,			/* "Pause!" */
	UiFontPauseFocused,		/* (yes | no) buttons in pause */
	UiFontPauseUnfocused,		/* not focused */
	UiFontPanelInfo,		/* panel info (score, lines, ...) */
	UiFontPanelValue,		/* panel value */

	UiFontLAST
};

/* Menus */
enum {
	UiMenuRoot,			/* root menu */
	UiMenuOptions,			/* options menu */
	UiMenuScores,			/* high scores */
	UiMenuQuit,			/* pause menu (yes | no) */
	UiMenuControls,			/* key controls */
	UiMenuJControls,		/* joystick controls */

	UiMenuLAST
};

/* Menus entry id */
enum {
	UiMnidNormal,			/* normal entity */
	UiMnidOption,			/* boolean option */
	UiMnidKey			/* key edit */
};

/*
 * Game colors, colors prefixed by (U) in the comment means that this
 * color may be updated during game.
 */
enum {
	UiColorBackground,		/* (U) normal background */
	UiColorGrid,			/* grid color in game */
	UiColorBoardBackground,		/* board background */
	UiColorBoardBorder,		/* game border color */
	UiColorPauseBackground,		/* pause background color */

	UiColorLAST
};

/* Keys */
enum {
	/* Keyboard */
	UiKeyDown = 0,
	UiKeyUp,
	UiKeyLeft,
	UiKeyRight,
	UiKeyRotLeft,
	UiKeyRotRight,
	UiKeyPause,
	UiKeyDrop,

	/* Joystick */
	UiKeyJRotLeft,
	UiKeyJRotRight,
	UiKeyJPause,
	UiKeyJDrop,
	UiKeyJActivate,
	UiKeyJCancel,

	UiKeyLAST
};
	
struct ui {
	SDL_Surface		*root;

	SDL_Surface		*shapes[UiShapeColorLAST];
	SDL_Surface		*board;
	SDL_Surface		*bgpause;
	SDL_Surface		*bgedit;
	SDL_Surface		*panel;

	/* Keys and joystick */
	Uint32			keys[UiKeyLAST];
	SDL_Joystick		*joy;

	/* Key and joystick control edit */
	bool			isedit_key;		/* is editing */
	int			*edit_key;		/* key to edit */

	bool			isedit_joy;		/* is editing */
	int			*edit_joy;		/* key to edit */

	/* SD-Tris title */
	SDL_Surface		*title;
	SDL_Surface		*title_s;
	SDL_Rect		title_geo;
	SDL_Rect		title_geos;

	/* Version at bottom */
	SDL_Surface		*version;
	SDL_Rect		version_geo;

	WG_Font			*fonts[UiFontLAST];

	WG_Menu			*menus[UiMenuLAST];
	WG_Menu			*menu;

	SDL_Color		colors[UiColorLAST];

	struct array		textbox;
	char			name[SCORE_MAXLEN + 1];
};

extern struct ui	ui;

void
ui_init(void);

void
ui_draw(void);

void
ui_quit(void);

#endif /* _UI_H_ */
#endif
