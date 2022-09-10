/*
 * os.c -- platform independant functions
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

#include "os.h"
#include "score.h"

/* {{{ Windows (WIN32) */

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define UNICODE	1

#  include <windows.h>
#  include <shlobj.h>

void
os_config_set_path(char *path, size_t max)
{
	HRESULT rv;
	TCHAR tmp[MAX_PATH] = { 0 };
	
	rv = SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, tmp);

	if (rv != S_OK)
		return;

	snprintf(path, max, "%ls\\sdtris\\conf.sdt", tmp);
}

void
os_score_set_path(char *path, size_t max)
{
	HRESULT rv;
	TCHAR tmp[MAX_PATH] = { 0 };

	rv = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
	    NULL, 0, tmp);

	if (rv != S_OK)
		return;

	snprintf(path, max, "%ls/sdtris", tmp);
}

void
os_data_set_path(char *path, size_t max)
{
	strncpy(path, "./", max);
}

void
os_nls_set_path(char *path, size_t max)
{
	strncpy(path, "./lang", max);
}

wchar_t *
os_get_name(void)
{
	DWORD len = SCORE_MAXLEN;
	static TCHAR name[SCORE_MAXLEN + 1] = { 0 };

	if (!GetUserName(name, &len))
		return L"";

	return name;
}

/* }}} */

/* {{{ Unix */

#else

void
os_config_set_path(char *path, size_t max)
{
	char *xdgconfig;

	if ((xdgconfig = getenv("XDG_CONFIG_DIR")) == NULL) {
		snprintf(path, max, "%s/.config/sdtris/", getenv("HOME"));
	} else {
		/* XDG_CONFIG_DIR is defined */
		snprintf(path, max, "%s/sdtris/", xdgconfig);
	}

	strncat(path, "conf.sdt", strlen(path) - max);
}

/*
 * For score, data and NLS, we use what cmake use as ${PREFIX} installation
 * path.
 */

void
os_score_set_path(char *path, size_t max)
{
	snprintf(path, max, "%s/scores.sdt", PATH_INSTALL_SCORE);
}

void
os_data_set_path(char *path, size_t max)
{
	snprintf(path, max, "%s", PATH_INSTALL_DATA);
}

void
os_nls_set_path(char *path, size_t max)
{
	snprintf(path, max, "%s", PATH_INSTALL_NLS);
}

wchar_t *
os_get_name(void)
{
	static wchar_t name[SCORE_MAXLEN + 1] = { 0 };
	char *tmp;

	if ((tmp = getlogin()) == NULL)
		return L"";

	if (mbstowcs(name, tmp, sizeof (name)) == (size_t)-1)
		return L"";

	return name;
}

#endif

/* }}} */
