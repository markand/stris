/*
 * ui.c -- user interface rendering
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

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "fonts/cartoon-relief.h"
#include "fonts/actionj.h"
#include "fonts/typography-ties.h"

#include "ui.h"
#include "util.h"
#include "tex.h"

SDL_Window *ui_win = NULL;
SDL_Renderer *ui_rdr = NULL;

static struct {
	const unsigned char *data;
	size_t datasz;
	int size;
	TTF_Font *font;
} fonts[] = {
	[UI_FONT_SPLASH] = {
		.data = data_fonts_typography_ties,
		.datasz = sizeof (data_fonts_typography_ties),
		.size = 58
	},
	[UI_FONT_TITLE] = {
		.data = data_fonts_actionj,
		.datasz = sizeof (data_fonts_actionj),
		.size = 80
	},
	[UI_FONT_MENU] = {
		.data = data_fonts_cartoon_relief,
		.datasz = sizeof (data_fonts_cartoon_relief),
		.size = 60
	}
};

static TTF_Font *
load_font(const unsigned char *data, size_t datasz, int size)
{
	SDL_RWops *ops;
	TTF_Font *font;

	if (!(ops = SDL_RWFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(font = TTF_OpenFontRW(ops, 1, size)))
		die("abort: %s\n", SDL_GetError());

	return font;
}

static inline void
load_fonts(void)
{
	for (size_t i = 0; i < LEN(fonts); ++i)
		fonts[i].font = load_font(fonts[i].data, fonts[i].datasz, fonts[i].size);
}

static void
render(struct tex *t, enum ui_font f, enum ui_palette color, const char *fmt, va_list ap)
{
	char buf[128];
	SDL_Color c = {
		.r = (color >> 24) & 0xff,
		.g = (color >> 16) & 0xff,
		.b = (color >> 8)  & 0xff
	};
	SDL_Surface *sf;

	vsnprintf(buf, sizeof (buf), fmt, ap);

	if (!(sf = TTF_RenderUTF8_Blended(fonts[f].font, buf, c)))
		die("abort: %s\n", SDL_GetError());
	if (!(t->handle = SDL_CreateTextureFromSurface(ui_rdr, sf)))
		die("abort: %s\n", SDL_GetError());
	if (SDL_QueryTexture(t->handle, NULL, NULL, &t->w, &t->h) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_FreeSurface(sf);
}

void
ui_init(void)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
		die("abort: %s\n", SDL_GetError());
	if (TTF_Init() < 0)
		die("abort: %s\n", SDL_GetError());
	if (SDL_CreateWindowAndRenderer(UI_W, UI_H, 0, &ui_win, &ui_rdr) < 0)
		die("abort: %s\n", SDL_GetError());

	SDL_SetWindowTitle(ui_win, "STris");

	load_fonts();
}

void
ui_set_color(enum ui_palette color)
{
	SDL_SetRenderDrawColor(ui_rdr,
	    (color >> 24) & 0xff,
	    (color >> 16) & 0xff,
	    (color >> 8)  & 0xff,
	    0xff
	);
}

void
ui_draw_background(void)
{
}

void
ui_render(struct tex *t, enum ui_font f, enum ui_palette color, const char *fmt, ...)
{
	assert(fmt);

	va_list ap;

	va_start(ap, fmt);
	render(t, f, color, fmt, ap);
	va_end(ap);
}

void
ui_clip(enum ui_font f, int *w, int *h, const char *fmt, ...)
{
	assert(w);
	assert(h);
	assert(fmt);

	struct tex tex;
	va_list ap;

	va_start(ap, fmt);
	render(&tex, f, 0, fmt, ap);
	va_end(ap);

	*w = tex.w;
	*h = tex.h;

	tex_finish(&tex);
}

void
ui_clear(void)
{
	SDL_RenderClear(ui_rdr);
}

void
ui_present(void)
{
	SDL_RenderPresent(ui_rdr);
}

void
ui_quit(void)
{
	SDL_DestroyRenderer(ui_rdr);
	SDL_DestroyWindow(ui_win);
	SDL_Quit();
}

#if 0

#include <wing/timer.h>
#include <wing/util.h>

#include "ui.h"
#include "action.h"
#include "draw.h"

static void	ui_init_fonts(void);
static void	ui_init_shapes(void);
static void	ui_init_menus(void);
static void	ui_init_pause(void);
static void	ui_init_controls(void);
static void	ui_init_title(void);
static void	ui_init_surfaces(void);

static void	ui_update_menu_font(void);
static void	ui_update_background(void);

enum {
	MaxMenuSel		= 150,	/* max menu entry g, b value */
	MinMenuSel		= 50	/* min menu entry g, b value */
};

/*
 * Background colors used for levels, when switchin to
 * a new level, the background will change to the next color.
 */

static const SDL_Color backgroundcolors[] = {
	{ 140, 185, 250	},			/* level 0 */
	{ 100, 125, 210	},			/* level 1 */
	{ 115, 210, 100	},			/* level 2 */
	{ 110, 175, 120	},			/* level 3 */
	{ 240, 230, 115	},			/* level 4 */
	{ 240, 200, 115	},			/* level 5 */
	{ 240, 165, 115	},			/* level 6 */
	{ 240, 120, 115	},			/* level 7 */
	{ 175, 175, 175	},			/* level 9 */
	{ 120, 120, 120	}			/* level 10 */
};

/* {{{ Fonts size and colors */

static int fontsizes[] = {
	[UiFontTitle]		= 50,
	[UiFontTitleShadow]	= 50,
	[UiFontMenu]		= 48,
	[UiFontMenuSelected]	= 48,
	[UiFontPause]		= 40,
	[UiFontPauseFocused]	= 20,
	[UiFontPauseUnfocused]	= 20,
	[UiFontPanelInfo]	= 10,
	[UiFontPanelValue]	= 10
};

static SDL_Color fontcolors[] = {
	[UiFontTitle]		= { 123, 73,  194	},
	[UiFontTitleShadow]	= { 142, 142, 142	},
	[UiFontMenu]		= { 235, 235, 235	},
	[UiFontMenuSelected]	= { 235, MaxMenuSel, MaxMenuSel	},
	[UiFontPause]		= { 246, 222, 143	},
	[UiFontPauseFocused]	= { 255, 255, 255	},
	[UiFontPauseUnfocused]	= { 204, 204, 204	},
	[UiFontPanelInfo]	= { 230, 230, 230	},
	[UiFontPanelValue]	= { 255, 255, 255	}
};

/* }}} */

/* --------------------------------------------------------
 * public functions and variables
 * -------------------------------------------------------- */

struct ui ui = {
	.keys = {
		/* Keyboard */
		[UiKeyDown]			= SDLK_DOWN,
		[UiKeyUp]			= SDLK_UP,
		[UiKeyLeft]			= SDLK_LEFT,
		[UiKeyRight]			= SDLK_RIGHT,
		[UiKeyRotLeft]			= SDLK_s,
		[UiKeyRotRight]			= SDLK_r,
		[UiKeyPause]			= SDLK_p,
		[UiKeyDrop]			= SDLK_SPACE,

		/* Good luck to have the same values than mine */
		[UiKeyJRotLeft]			= 2,
		[UiKeyJRotRight]		= 3,
		[UiKeyJPause]			= 9,
		[UiKeyJDrop]			= 0,
		[UiKeyJActivate]		= 1,
		[UiKeyJCancel]			= 9,
	},

	/* We are not editing a key control */
	.isedit_key	= false,
	.isedit_joy	= false,

	.colors = {
		[UiColorBackground]		= { 140, 185, 250	},
		[UiColorGrid]			= { 230, 230, 230	},
		[UiColorBoardBackground]	= { 235, 245, 250	},
		[UiColorBoardBorder]		= { 149, 195, 221	},
		[UiColorPauseBackground]	= { 123, 123, 123	},
	}
};

void
ui_init(void)
{
	int flags;

	flags = DEF_SDLFLAGS;
	if (sdtris.audio)
		flags |= SDL_INIT_AUDIO;

	if (SDL_Init(flags) < 0)
		errx(1, "%s", SDL_GetError());
	if (!(IMG_Init(DEF_IMGFLAGS) & (DEF_IMGFLAGS)))
		errx(1, "%s", SDL_GetError());
	if (TTF_Init() < 0)
		errx(1, "%s", SDL_GetError());

	if ((ui.joy = SDL_JoystickOpen(0)))
		printf("Joystick supported: %s\n", SDL_JoystickName(0));

	WG_set(WG_SET_MALLOC, xmalloc);
	WG_set(WG_SET_CALLOC, xcalloc);
	WG_set(WG_SET_REALLOC, xrealloc);

	/* Caption and icon */
	SDL_WM_SetCaption("SD-Tris", NULL);
	SDL_WM_SetIcon(IMG_Load(getdata("win-icon.png")), NULL);

	SDL_EnableUNICODE(1);

	flags = DEF_VIDEOFLAGS;
	if (sdtris.fullscreen)
		flags |= SDL_FULLSCREEN;

	/* Screen */
	ui.root = SDL_SetVideoMode(DEF_WIDTH, DEF_HEIGHT, 32, flags);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
	    SDL_DEFAULT_REPEAT_INTERVAL);

	if (!ui.root)
		errx(1, "%s", SDL_GetError());

	ui_init_fonts();
	ui_init_shapes();
	ui_init_menus();
	ui_init_pause();
	ui_init_controls();
	ui_init_title();
	ui_init_surfaces();

	/* Text field for high score */
	array_set(&ui.textbox, "lfmr", SCORE_MAXLEN, ARRAY_FIXED,
	    xmalloc, xrealloc);
	array_init(&ui.textbox, sizeof (uint16_t));
}

void
ui_draw(void)
{
	ui_update_menu_font();
	ui_update_background();

	draw_background();

	if (game.state == GameMenu)
		draw_menus();
	else if (game.state == GameRunning || game.state == GamePaused)
		draw_game();
	else if (game.state == GameDead)
		draw_dead();

	SDL_Flip(ui.root);
}

void
ui_quit(void)
{
	int i;

	for (i = 0; i < UiShapeColorLAST; ++i)
		SDL_FreeSurface(ui.shapes[i]);

	SDL_FreeSurface(ui.board);
	SDL_FreeSurface(ui.bgpause);
	SDL_FreeSurface(ui.panel);

	SDL_FreeSurface(ui.title);
	SDL_FreeSurface(ui.title_s);
	SDL_FreeSurface(ui.version);

	if (ui.joy != NULL)
		SDL_JoystickClose(ui.joy);

	SDL_Quit();
}

/* --------------------------------------------------------
 * private functions
 * -------------------------------------------------------- */

/* ---
 * Init functions
 */

/* {{{ Fonts */

static void
ui_init_fonts(void)
{
	#define FONT_DEF(t, p, s)					\
		{ &ui.fonts[t], p, fontsizes[t], s, &fontcolors[t]	}

	struct {
		WG_Font		**ft;
		const char	*file;
		int		size;
		int		style;
		SDL_Color	*color;
	} *f, fonts[] = {
		/* SD-Tris title */
		FONT_DEF(UiFontTitle, "actionj.ttf", TTF_STYLE_NORMAL),
		FONT_DEF(UiFontTitleShadow, "actionj.ttf", TTF_STYLE_NORMAL),

		/* Menu entry font */
		FONT_DEF(UiFontMenu, "Cartoon Relief.ttf", TTF_STYLE_NORMAL),
		FONT_DEF(UiFontMenuSelected, "Cartoon Relief.ttf", TTF_STYLE_NORMAL),

		/* Pause! */
		FONT_DEF(UiFontPause, "Go Boom.ttf", TTF_STYLE_NORMAL),
		FONT_DEF(UiFontPauseFocused, "DejaVuSans.ttf", TTF_STYLE_NORMAL),
		FONT_DEF(UiFontPauseUnfocused, "DejaVuSans.ttf", TTF_STYLE_NORMAL),

		/* Panel info and values */
		FONT_DEF(UiFontPanelInfo, "DejaVuSans.ttf", TTF_STYLE_BOLD),
		FONT_DEF(UiFontPanelValue, "DejaVuSans.ttf", TTF_STYLE_NORMAL),

		{ NULL, NULL, },
	};

	for (f = fonts; f->ft; ++f) {
		*f->ft = WG_fontCreate(getdata("fonts/%s", f->file), f->size,
		    WG_FONT_BLENDED, NULL, f->color);

		if (!*f->ft)
			errx(1, "%s", WG_getError());

		TTF_SetFontStyle((*f->ft)->font, f->style);
	}
}

/* }}} */

/* {{{ Shapes */

static void
ui_init_shapes(void)
{
	static const struct {
		SDL_Surface	**sf;
		const char	*file;
	} *s, images[] = {
		{ &ui.shapes[UiShapeColorBlue],		"blue.png"	},
		{ &ui.shapes[UiShapeColorCyan],		"cyan.png"	},
		{ &ui.shapes[UiShapeColorGreen],	"green.png"	},
		{ &ui.shapes[UiShapeColorMagenta],	"purple.png"	},
		{ &ui.shapes[UiShapeColorOrange],	"orange.png"	},
		{ &ui.shapes[UiShapeColorRed],		"red.png"	},
		{ &ui.shapes[UiShapeColorYellow],	"yellow.png"	},
		{ NULL, NULL }
	};

	for (s = images; s->sf; ++s) {
		if (!(*s->sf = IMG_Load(getdata("/img/%s", s->file))))
			errx(1, "%s", SDL_GetError());
	}
}

/* }}} */

/* {{{ Menu initializations */

/*
 * Compute total height in pixels of entries in the current menu,
 * max specify max entries to read, space is the space between entries.
 */

static int
get_total_height(WG_Menu *menu, WG_Font *font, int max, int space)
{
	WG_MenuEntry *e;

	int i, h, total = 0;

	for (i = 0; i < max && i < menu->nentries; ++i) {
		e = &menu->entries[i];

		WG_fontSizeText(font, NULL, &h, e->label);
		total += h;
	}

	return total + (i * space);
}

/*
 * This set the coordinates of the text to be rendered, it compute
 * the width, height and position to be printed on the center of the
 * screen.
 */

static void
set_menu_center_coords(WG_Menu *menu, int begin, int end)
{
	WG_MenuEntry *e;
	int totalh, y, i;

	/* Set ui elements */
	totalh	= get_total_height(menu, ui.fonts[UiFontMenu], menu->nentries, 15);
	y	= (ui.root->h - totalh) / 2;

	e = menu->entries;
	for (i = begin; i < end && i < menu->nentries; ++i, ++e) {
		WG_fontSizeText(ui.fonts[UiFontMenu], &e->w, &e->h, e->label);

		/* Compute the center for each entries */
		e->y	= y;
		e->x	= (ui.root->w - e->w) / 2;

		y += e->h + 15;
	}
}

/*
 * Set the "Back" button to the center-bottom of the screen, the back menu entry
 * must *mandatory* be the last element of the menu.
 */

static void
set_menu_back_coords(WG_Menu *menu)
{
	WG_MenuEntry *e;

	e = &menu->entries[menu->nentries - 1];

	WG_fontSizeText(ui.fonts[UiFontMenu], &e->w, &e->h, e->label);

	e->x = (ui.root->w - e->w) / 2;
	e->y = (ui.root->h - e->h) - 10;
}

/*
 * Set coordinates for controls, menu->nentries -1 will be updated,
 * the last one is not set because it is the "Back" button.
 */

static void
set_controls_geo(WG_Menu *menu)
{
	int i, y, h;

	WG_MenuEntry *e;

	y = 10;
	for (i = 0; i < menu->nentries - 1; ++i) {
		e = &menu->entries[i];
		WG_fontSizeText(ui.fonts[UiFontPauseFocused], NULL, &h,
		    e->label);

		e->x = 10;
		e->y = y;

		e->w = ui.root->w - 20;
		e->h = h;

		/* Pad entry + 10 */
		y += h + 10;
	}
}

#define APP_NORMAL(str, act, value)					\
	WG_menuAppend(m, str, (action_t)act, value, UiMnidNormal)
#define APP_OPTION(str, act, value)					\
	WG_menuAppend(m, str, (action_t)act, value, UiMnidOption)
#define APP_KEY(str, act, value)					\
	WG_menuAppend(m, str, (action_t)act, value, UiMnidKey)

static void
init_menus_options(void)
{
	WG_Menu *m;

	ui.menus[UiMenuOptions] = WG_menuCreate(UiMenuOptions, NULL,
	    WG_MENU_LOOP, ui.menus[UiMenuRoot]);

	m = ui.menus[UiMenuOptions];

	APP_OPTION(_("Fullscreen"), action_toggle_fullscreen, &sdtris.fullscreen);
	APP_OPTION(_("Audio"), action_toggle_audio, &sdtris.audio);

	APP_NORMAL(_("Keyboard"), action_select, &ui.menus[UiMenuControls]);
	APP_NORMAL(_("Joystick"), action_select, &ui.menus[UiMenuJControls]);

	/* Special back button */
	APP_NORMAL(_("Back"), action_set_parent, NULL);

	/* Apply center only for options */
	set_menu_center_coords(m, 0, m->nentries - 1);
	set_menu_back_coords(m);
}

static void
init_menus_scores(void)
{
	WG_Menu *m;

	ui.menus[UiMenuScores] = WG_menuCreate(UiMenuScores, NULL, 0,
	    ui.menus[UiMenuRoot]);

	m = ui.menus[UiMenuScores];

	/* Special back button */
	APP_NORMAL(_("Back"), action_set_parent, NULL);
	set_menu_back_coords(m);
}

static void
init_menus_controls(void)
{
	WG_Menu *m;

	ui.menus[UiMenuControls] = WG_menuCreate(UiMenuControls, NULL,
	    WG_MENU_LOOP, ui.menus[UiMenuOptions]);

	m = ui.menus[UiMenuControls];

	APP_KEY(_("Down"), action_set_key, &ui.keys[UiKeyDown]);
	APP_KEY(_("Up"), action_set_key, &ui.keys[UiKeyUp]);
	APP_KEY(_("Left"), action_set_key, &ui.keys[UiKeyLeft]);
	APP_KEY(_("Right"), action_set_key, &ui.keys[UiKeyRight]);
	APP_KEY(_("Rotate left"), action_set_key, &ui.keys[UiKeyRotLeft]);
	APP_KEY(_("Rotate right"), action_set_key, &ui.keys[UiKeyRotRight]);
	APP_KEY(_("Pause"), action_set_key, &ui.keys[UiKeyPause]);
	APP_KEY(_("Drop"), action_set_key, &ui.keys[UiKeyDrop]);

	/* Special back button */
	APP_NORMAL(_("Back"), action_set_parent, NULL);

	set_controls_geo(m);
	set_menu_back_coords(m);
}

static void
init_menus_jcontrols(void)
{
	WG_Menu *m;

	ui.menus[UiMenuJControls] = WG_menuCreate(UiMenuJControls, NULL,
	    WG_MENU_LOOP, ui.menus[UiMenuOptions]);

	m = ui.menus[UiMenuJControls];

	APP_KEY(_("Rotate left"), action_set_joy, &ui.keys[UiKeyJRotLeft]);
	APP_KEY(_("Rotate right"), action_set_joy, &ui.keys[UiKeyJRotRight]);
	APP_KEY(_("Pause"), action_set_joy, &ui.keys[UiKeyJPause]);
	APP_KEY(_("Drop"), action_set_joy, &ui.keys[UiKeyJDrop]);
	APP_KEY(_("Activate"), action_set_joy, &ui.keys[UiKeyJActivate]);
	APP_KEY(_("Cancel"), action_set_joy, &ui.keys[UiKeyJCancel]);

	APP_NORMAL(_("Back"), action_set_parent, NULL);
	set_controls_geo(m);
	set_menu_back_coords(m);
}

static void
init_menus_quit(void)
{
	WG_Menu *m;
	WG_MenuEntry *e;

	ui.menus[UiMenuQuit] = WG_menuCreate(UiMenuQuit, NULL,
	    WG_MENU_LOOP, ui.menus[UiMenuRoot]);

	m = ui.menus[UiMenuQuit];

	/* Use Key to have key font */
	APP_KEY(_("Yes"), action_goto_root, NULL);
	APP_KEY(_("No"), action_cancel_pause, NULL);

	/* Default to "No" */
	WG_menuSelectIndex(m, 1);

	m->entries[0].y = m->entries[1].y = 250;

	/* Set "Yes" | "No" */
	e = &m->entries[0];
	WG_fontSizeText(ui.fonts[UiFontPauseFocused], &e->w, &e->h, e->label);
	e->x = (ui.root->w / 4) - (e->w / 2);

	e = &m->entries[1];
	WG_fontSizeText(ui.fonts[UiFontPauseFocused], &e->w, &e->h, e->label);
	e->x = ((ui.root->w - e->w) / 2) + (ui.root->w / 4);
}

static void
init_menus_root(void)
{
	WG_Menu *m;

	ui.menus[UiMenuRoot] = WG_menuCreate(UiMenuRoot, 0, WG_MENU_LOOP, 0);

	m = ui.menus[UiMenuRoot];

	APP_NORMAL(_("Play"), action_play, NULL);
	APP_NORMAL(_("Options"), action_select, &ui.menus[UiMenuOptions]);
	APP_NORMAL(_("High scores"), action_select, &ui.menus[UiMenuScores]);
	APP_NORMAL(_("Quit"), action_quit, NULL);

	/* Apply all to the center */
	set_menu_center_coords(m, 0, m->nentries);
}

static void
ui_init_menus(void)
{
	init_menus_root();
	init_menus_options();
	    init_menus_controls();
	    init_menus_jcontrols();
	init_menus_scores();
	init_menus_quit();

	ui.menu = ui.menus[UiMenuRoot];
}

/* }}} */

/* {{{ Pause */

static void
ui_init_pause(void)
{
	SDL_Surface *sf;
	SDL_Rect geo;
	SDL_Color *c;

	/* Background for pausing */
	ui.bgpause = SDL_CreateRGBSurface(SDL_HWSURFACE, DEF_WIDTH,
	    DEF_HEIGHT, 32, 0, 0, 0, 0);

	if (!ui.bgpause)
		errx(1, "%s", SDL_GetError());

	SDL_SetAlpha(ui.bgpause, SDL_SRCALPHA, 175);

	c = &ui.colors[UiColorPauseBackground];
	SDL_FillRect(ui.bgpause, NULL,
	    SDL_MapRGB(ui.bgpause->format, c->r, c->g, c->b));

	/* Write "Pause" */
	sf = WG_fontRender(ui.fonts[UiFontPause], "Pause");

	geo.x	= (ui.root->w - sf->w) / 2;
	geo.y	= ((ui.root->h - sf->h) / 2) - 50;

	SDL_BlitSurface(sf, NULL, ui.bgpause, &geo);
	SDL_FreeSurface(sf);

	/* Write "Go back to main menu?" */
	sf = WG_fontRender(ui.fonts[UiFontPauseFocused], _("Go to the main menu?"));

	geo.x	= (ui.root->w - sf->w) / 2;
	geo.y	= (ui.root->h - sf->h) / 2;

	SDL_BlitSurface(sf, NULL, ui.bgpause, &geo);
	SDL_FreeSurface(sf);
}

/* }}} */

/* {{{ Edit key */

static void
ui_init_controls(void)
{
	SDL_Surface *sf;
	SDL_Rect geo;

	/* Background for editing */
	ui.bgedit = SDL_CreateRGBSurface(SDL_HWSURFACE, DEF_WIDTH,
	    DEF_HEIGHT, 32, 0, 0, 0, 0);

	if (!ui.bgedit)
		errx(1, "%s", SDL_GetError());

	SDL_SetAlpha(ui.bgedit, SDL_SRCALPHA, 120);

	sf = WG_fontRender(ui.fonts[UiFontPause], _("Edit key"));
	geo.x	= (ui.root->w - sf->w) / 2;
	geo.y	= ((ui.root->h - sf->h) / 2) - 60;

	SDL_BlitSurface(sf, NULL, ui.bgedit, &geo);
	SDL_FreeSurface(sf);

	sf = WG_fontRender(ui.fonts[UiFontPauseFocused],
	    _("Press key or backspace to cancel"));

	geo.x	= (ui.root->w - sf->w) / 2;
	geo.y	= (ui.root->h - sf->h) / 2;

	SDL_BlitSurface(sf, NULL, ui.bgedit, &geo);
	SDL_FreeSurface(sf);
}

/* }}} */

/* {{{ Title */

static void
ui_init_title(void)
{
	ui.title	= WG_fontRender(ui.fonts[UiFontTitle], "SD-Tris");
	ui.title_s	= WG_fontRender(ui.fonts[UiFontTitleShadow], "SD-Tris");
	ui.version	= WG_fontRender(ui.fonts[UiFontPanelValue], VERSION);

	if (!ui.title || !ui.title_s || !ui.version)
		errx(1, "%s", SDL_GetError());

	ui.title_geo.x	= (ui.root->w - ui.title->w) / 2;
	ui.title_geo.y	= 10;

	ui.title_geos	= ui.title_geo;
	ui.title_geos.x	++;
	ui.title_geos.y	++;

	ui.version_geo.x = ui.root->w - ui.version->w - 10;
	ui.version_geo.y = ui.root->h - ui.version->h - 10;
}

/* }}} */

/* {{{ Surfaces */

static void
ui_init_surfaces(void)
{
	/* Board for playing */
	ui.board = SDL_CreateRGBSurface(SDL_HWSURFACE,
	    (BoardWidth * 20) + 100, BoardHeight * 20, 32, 0, 0, 0, 0);

	/* Right panel */
	ui.panel = IMG_Load(getdata("img/panel.png"));

	if (!ui.board || !ui.panel)
		errx(1, "%s", SDL_GetError());
}

/* }}} */

/* ---
 * Update functions
 */

/*
 * Update the selected entry font to show which menu entry is
 * currently selected
 */

static void
ui_update_menu_font(void)
{
	static int8_t	direction = 1;
	static WG_Timer	timer;

	WG_Font *font = ui.fonts[UiFontMenuSelected];

	if (WG_timerElapsed(&timer) >= 20) {
		font->fg.g += direction;
		font->fg.b += direction;

		if (direction > 0 && font->fg.b >= MaxMenuSel)
			direction = -2;
		else if (direction < 0 && font->fg.b <= MinMenuSel)
			direction = 2;

		WG_timerStart(&timer);
	}
}

/*
 * Update the background color, depending on the game.level, if game.level
 * change the background will be set to the next level.
 */

static void
ui_update_background(void)
{
	static int	level;
	static WG_Timer	timer;

	SDL_Color	*bg;		/* current background */
	const SDL_Color	*c;		/* new background */

	/* Direction to update colors */
	int dr, dg, db;

	if (game.level == level)
		return;

	/* New level */
	c	= &backgroundcolors[game.level];
	bg	= &ui.colors[UiColorBackground];

	dr = (c->r > bg->r) ? 1 : -1;
	dg = (c->g > bg->g) ? 1 : -1;
	db = (c->b > bg->b) ? 1 : -1;

	/* Update finished */
	if (bg->r == c->r && bg->g == c->g && bg->b == c->b) {
		level = game.level;
		return;
	}

	if (WG_timerElapsed(&timer) >= 20) {
		if (bg->r != c->r)
			bg->r += dr;
		if (bg->g != c->g)
			bg->g += dg;
		if (bg->b != c->b)
			bg->b += db;

		WG_timerStart(&timer);
	}
}
#endif
