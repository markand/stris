/*
 * sound.c -- sound support and registry
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

#include <SDL3_mixer/SDL_mixer.h>

#include "sound/clean.h"
#include "sound/drop.h"
#include "sound/move.h"
#include "sound/startup.h"
#include "sound/tick.h"

#include "sound.h"
#include "stris.h"
#include "util.h"

#define SOUND_DEF(d) { .data = d, .datasz = sizeof (d) }

static struct {
	MIX_Audio *snd;
	MIX_Track *track;
	const void *data;
	size_t datasz;
} sounds[] = {
	[SOUND_CHIME]   = SOUND_DEF(assets_sound_startup),
	[SOUND_MOVE]    = SOUND_DEF(assets_sound_move),
	[SOUND_DROP]    = SOUND_DEF(assets_sound_drop),
	[SOUND_CLEAN]   = SOUND_DEF(assets_sound_clean),
	[SOUND_TICK]    = SOUND_DEF(assets_sound_tick)
};

static MIX_Mixer *mixer;

static void
load_sound(enum sound snd)
{
	SDL_IOStream *src;

	if (!(src = SDL_IOFromConstMem(sounds[snd].data, sounds[snd].datasz)))
		die("SDL_IOFromConstMem: %s\n", SDL_GetError());
	if (!(sounds[snd].snd = MIX_LoadAudio_IO(mixer, src, true, true)))
		die("MIX_LoadAudio_IO: %s\n", SDL_GetError());
	if (!(sounds[snd].track = MIX_CreateTrack(mixer)))
		die("MIX_CreateTrack: %s\n", SDL_GetError());
	if (!MIX_SetTrackAudio(sounds[snd].track, sounds[snd].snd))
		die("MIX_SetTrackAudio: %s\n", SDL_GetError());
}

void
sound_init(void)
{
	if (!(mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL)))
		die("abort: %s\n", SDL_GetError());

	for (size_t i = 0; i < LEN(sounds); ++i)
		load_sound(i);
}

void
sound_play(enum sound snd)
{
	if (sconf.sound)
		MIX_PlayTrack(sounds[snd].track, 0);
}

void
sound_finish(void)
{
	for (size_t i = 0; i < LEN(sounds); ++i) {
		MIX_DestroyTrack(sounds[i].track);
		MIX_DestroyAudio(sounds[i].snd);
	}

	MIX_DestroyMixer(mixer);
}
