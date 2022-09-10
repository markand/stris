/*
 * sys.h -- system routines
 *
 * Copyright (c) 2020 David Demelier <markand@malikania.fr>
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

#ifndef MOLKO_CORE_SYS_H
#define MOLKO_CORE_SYS_H

#include <stdarg.h>
#include <stdbool.h>

enum sys_dir {
	SYS_DIR_BIN,
	SYS_DIR_DATA,
	SYS_DIR_LOCALE,
	SYS_DIR_SAVE,
};

bool
sys_init(const char *organization, const char *name);

const char *
sys_dir(enum sys_dir kind);

bool
sys_mkdir(const char *path);

void
sys_finish(void);

#endif /* !MOLKO_CORE_SYS_H */
