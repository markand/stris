STris INSTALL
=============

Installation instructions.

Requirements
------------

- C23 compliant compiler.
- [SDL2][], Multimedia library.
- [SDL2_image][], Image loading addon for SDL2.
- [SDL2_mixer][], Audio addon for SDL2.
- [SDL2_ttf][], Fonts addon for SDL2,

STris has been tested successfully on the following systems:

- OpenBSD 7.1,
- Alpine Linux 3.16,
- Arch Linux,
- macOS Ventura,
- Windows 11 (MSYS2 only).

Basic installation
------------------

Quick install.

    $ bsdtar -xvf stris-x.y.z-tar.xz
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

[SDL2]: http://libsdl.org
[SDL2_image]: https://www.libsdl.org/projects/SDL_image
[SDL2_mixer]: https://www.libsdl.org/projects/SDL_mixer
[SDL2_ttf]: https://www.libsdl.org/projects/SDL_ttf
