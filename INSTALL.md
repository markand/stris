STris INSTALL
=============

Installation instructions.

Requirements
------------

- C23 compliant compiler.
- [cmake][], CMake.
- [SDL3][], Multimedia library.
- [SDL3_image][], Image loading addon for SDL3.
- [SDL3_mixer][], Audio addon for SDL3.
- [SDL3_ttf][], Fonts addon for SDL3,

STris has been tested successfully on the following systems:

- OpenBSD 7.2,
- Alpine Linux 3.17,
- Arch Linux,
- macOS Ventura,
- Windows 11 (MSYS2, Visual Studio 2022).

Basic installation
------------------

Quick install.

    $ tar -xvf stris-x.y.z-tar.xz
    $ cd stris-x.y.z
    $ make
    # sudo make install

On most UNIX systems, you need to install the game if you want to register high
scores. This is done using the setgid permission so every user on the same
machine can update the score database file.

You can tweak the installation with the following make variables at compile and
install time:

- `PREFIX`: default root directory for installation (default: /usr/local).
- `BINDIR`: programs directory (default: ${PREFIX}/bin).
- `MANDIR`: manual pages directory (default: ${PREFIX}/share/man).
- `VARDIR`: database and logs directory (default: ${PREFIX}/var).

To alter the setgid permissions, use the following variables:

- `UID`: owner uid/name (default: root).
- `GID`: group uid/name (default: games).

Platform: Windows
-----------------

On Windows, [MSYS2][] and Visual Studio are supported.

### MSYS2

Once you have MSYS2 installed, simply install the following packages from the
appropriate MinGW shell prior to the chapter above.

- *mingw-w64-clang-x86_64-cmake*
- *mingw-w64-clang-x86_64-SDL3*
- *mingw-w64-clang-x86_64-SDL3_image*
- *mingw-w64-clang-x86_64-SDL3_mixer*
- *mingw-w64-clang-x86_64-SDL3_ttf*

Note: replace `x86_64` with `i686` if you have a deprecated system or if you
      have issues while debugging (MinGW-w64 and/or gdb have known issues in
      this area).

Remove `clang-` prefix if you need to use gcc for some reasons.

### Visual Studio

With Visual Studio you will have to build/download libraries yourself or use the
[prebuilt][devkit] one specifically for stris (it may contains other unused
libraries though).

Then, make sure to have a common root prefix for all libraries in a folder of
choice:

- <dir>/bin
- <dir>/lib
- <dir>/include

Use CMake to generate IDE files of your choice. Use `CMAKE_PREFIX_PATH` pointing
to *dir* and add *dir/bin* to your `PATH` to help CMake finding libraries.

[MSYS2]: https://www.msys2.org
[SDL3]: http://libsdl.org
[SDL3_image]: https://www.libsdl.org/projects/SDL_image
[SDL3_mixer]: https://www.libsdl.org/projects/SDL_mixer
[SDL3_ttf]: https://www.libsdl.org/projects/SDL_ttf
[cmake]: https://cmake.org
[devkit]: https://releases.malikania.fr/devkit
