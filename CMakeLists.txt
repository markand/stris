cmake_minimum_required(VERSION 3.24)
project(
	stris
	VERSION 0.5.0
	DESCRIPTION "Simple Tetris"
	HOMEPAGE_URL "http://projects.malikania.fr/stris"
	LANGUAGES "C"
)

include(GNUInstallDirs)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(SDL3_ttf REQUIRED)

find_library(MATH_LIBRARY m)

add_subdirectory(extern/bcc)
add_subdirectory(assets)
add_subdirectory(src)
