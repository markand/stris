/*
 * sound.c -- simple advertising splash screen
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

#include <SDL_mixer.h>

#include "sound/drop.h"
#include "sound/move.h"
#include "sound/rotate.h"
#include "sound/startup.h"

#include "sound.h"
#include "util.h"

#define SOUND_DEF(d) { .data = d, .datasz = sizeof (d) }

static struct {
	Mix_Chunk *snd;
	const void *data;
	size_t datasz;
} sounds[] = {
	// https://freesound.org/people/radian/sounds/63000
	[SOUND_CHIME] = SOUND_DEF(data_sound_startup),
	// https://freesound.org/people/el_boss/sounds/560701
	[SOUND_ROTATE] = SOUND_DEF(data_sound_rotate),
	// https://freesound.org/people/MissCellany/sounds/240640
	[SOUND_MOVE] = SOUND_DEF(data_sound_move),
	// https://freesound.org/people/MattRuthSound/sounds/562083
	[SOUND_DROP] = SOUND_DEF(data_sound_drop)
};

static Mix_Chunk *
load_sound(const void *data, size_t datasz)
{
	SDL_RWops *src;
	Mix_Chunk *snd;

	if (!(src = SDL_RWFromConstMem(data, datasz)))
		die("abort: %s\n", SDL_GetError());
	if (!(snd = Mix_LoadWAV_RW(src, 1)))
		die("abort: %s\n", SDL_GetError());

	return snd;
}

void
sound_init(void)
{
	// No special file types yet.
	Mix_Init(0);

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) < 0)
		die("abort: %s\n", SDL_GetError());

	Mix_AllocateChannels(4);

	for (size_t i = 0; i < LEN(sounds); ++i)
		sounds[i].snd = load_sound(sounds[i].data, sounds[i].datasz);
}

void
sound_play(enum sound snd)
{
	Mix_PlayChannel(-1, sounds[snd].snd, 0);
}

void
sound_finish(void)
{
	for (size_t i = 0; i < LEN(sounds); ++i)
		Mix_FreeChunk(sounds[i].snd);

	Mix_CloseAudio();
	Mix_Quit();
}
