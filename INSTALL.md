STris INSTALL
=============

Installation instructions.

Requirements
------------

- C23 compliant compiler.
- GNU make,
- [SDL2][], Multimedia library.
- [SDL2_image][], Image loading addon for SDL2.
- [SDL2_mixer][], Audio addon for SDL2.
- [SDL2_ttf][], Fonts addon for SDL2,

STris has been tested successfully on the following systems:

- OpenBSD 7.1,
- Alpine Linux 3.16,
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

- *make*
- *mingw-w64-clang-x86_64-SDL2*
- *mingw-w64-clang-x86_64-SDL2_image*
- *mingw-w64-clang-x86_64-SDL2_mixer*
- *mingw-w64-clang-x86_64-SDL2_ttf*

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

Building with Visual Studio is only supported through `clang-cl` which should be
installed as individual component from the Visual Studio installer. You also
need a POSIX compliant toolset such as [MSYS2][] and add it to your path.

Open a X64 native command prompt from the Visual Studio 2022 start up menu, then
invoke `C:\msys64\msys2_shell -use-full-path` (adjust where you have installed
MSYS2).

Then, with this mintty window you should have `make` command and `clang-cl`
available as well. We need to create a *config-clang-cl.mk* to indicate the path
to the libraries.

Create a *config-clang-cl.mk* file with the following at the top of the stris directory:

    # Adjust this variable where you have your libraries, change debug/release
    # depending on the build you wish.
    PREFIX := C:/pkg/vs/debug

Now build using the *Makefile.clang-cl* file rather than the plain *Makefile*:

    make -f Makefile.clang-cl

[MSYS2]: https://www.msys2.org
[SDL2]: http://libsdl.org
[SDL2_image]: https://www.libsdl.org/projects/SDL_image
[SDL2_mixer]: https://www.libsdl.org/projects/SDL_mixer
[SDL2_ttf]: https://www.libsdl.org/projects/SDL_ttf
[devkit]: https://releases.malikania.fr/devkit
