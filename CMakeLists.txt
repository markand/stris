#
# CMakeLists.txt -- CMake build system for STris
#
# Copyright (c) 2011-2024 David Demelier <markand@malikania.fr>
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

cmake_minimum_required(VERSION 3.24)
project(
	stris
	VERSION 0.5.0
	DESCRIPTION "Simple Tetris"
	HOMEPAGE_URL "http://projects.malikania.fr/stris"
	LANGUAGES "C"
)

set_property(GLOBAL PROPERTY USE_FOLDERS On)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED On)
set(CMAKE_C_EXTENSIONS On)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	set(CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")

	if (CMAKE_C_COMPILER_ID MATCHES "GNU")
		set(CMAKE_C_FLAGS "-Wno-format-truncation ${CMAKE_C_FLAGS}")
	else ()
		set(CMAKE_C_FLAGS "-Wno-fixed-enum-extension ${CMAKE_C_FLAGS}")
	endif ()
elseif (CMAKE_C_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_C_FLAGS "/W3 /wd4090 /wd4244 /wd4267 /wd4996 /wd5105 /wd6031 /wd6001 /wd26451 ${CMAKE_C_FLAGS}")
	set(CMAKE_C_FLAGS "/D_CRT_SECURE_NO_WARNINGS ${CMAKE_C_FLAGS}")
endif ()

include(GNUInstallDirs)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

find_library(MATH_LIBRARY m)

add_subdirectory(extern/bcc)
add_subdirectory(assets)
add_subdirectory(src)
