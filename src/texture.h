/*
 * tex.h -- texture management
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

#ifndef STRIS_TEXTURE_H
#define STRIS_TEXTURE_H

#include <stddef.h>
#include <stdint.h>

/**
 * \struct texture
 * \brief Rendering texture.
 */
struct texture {
	/**
	 * (read-only)
	 *
	 * Texture width.
	 */
	unsigned int w;

	/**
	 * (read-only)
	 *
	 * Texture height.
	 */
	unsigned int h;

	void *handle;   /* SDL_Texture */
};

/**
 * Create a new empty texture.
 *
 * To draw on the texture use ::UI_BEGIN and ::UI_END or the low-level
 * ::ui_target variant.
 *
 * \param width requested width (not 0)
 * \param height requested height (not 0)
 */
void
texture_init(struct texture *texture, unsigned int width, unsigned int height);

/**
 * Create a texture from existing image.
 *
 * The data must remain valid until texture is no longer needed.
 *
 * \param data the image content (not NULL)
 * \param datasz the image size
 */
void
texture_load(struct texture *texture, const void *data, size_t datasz);

/**
 * Draw the texture 1:1 at the x;y coordinates.
 */
void
texture_render(struct texture *texture, int x, int y);

/**
 * Scale the texture to w:h at the x;y coordinates.
 */
void
texture_scale(struct texture *texture, int x, int y, unsigned int w, unsigned int h);

/**
 * Change texture transparency.
 *
 * \param alpha the new transparency (in range 0..255)
 */
void
texture_alpha(struct texture *texture, unsigned int alpha);

/**
 * Apply color modulation by adding `color` to current blend mode.
 *
 * \param color the color to apply
 */
void
texture_color_add(struct texture *texture, uint32_t color);

void
texture_color_blend(struct texture *texture, uint32_t color);

void
texture_color_multiply(struct texture *texture, uint32_t color);

void
texture_color_none(struct texture *texture);

/**
 * Destroy the texture.
 */
void
texture_finish(struct texture *texture);

#endif /* !STRIS_TEXTURE_H */
