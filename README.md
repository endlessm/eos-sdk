Open Endless SDK
================

Libraries and development tools for building Endless applications.

Coding style
------------

We follow the GTK coding style:
https://git.gnome.org/browse/gtk+/plain/docs/CODING-STYLE

Build instructions
------------------

Run `make` anytime you change a file.

Preparing the source
--------------------

From a fresh source checkout, run:

    ./autogen.sh
    ./configure

This prepares the source tree for building. The `configure` script takes several options, common ones being:

 - `--prefix=/usr`: install in `/usr` instead of `/usr/local` (you probably want this)
 - `--enable-gtk-doc`: generate the documentation whenever you build; this makes your build slower, but the documentation will always be up to date
 - If you're doing a debug build, then `CFLAGS="-g -O0" ./configure` will make sure that `make` always builds with debugging symbols and optimization off.

Then, run `make` to rebuild, and `sudo make install` to copy the SDK into the system.

By default, the compiler invocations with all the flags are not shown.
If you want to see them, run `make V=1`.
If you want make to try and run several jobs in parallel, run `make -j2` for 2 jobs, for example.

Testing
-------

To run the automated tests, run `make check`.

Packaging
---------

Before packaging, run `make distcheck`.
This runs the tests as above, but from a clean source tree in a temporary location.
It also installs all the files in a temporary location and uninstalls them again, in order to make sure that nothing is forgotten.

To build a Debian package, run `make debian`.

Fixing breakage
---------------

If at any time the state of your build tree gets wedged, then try the following:

    make clean
    make

If that doesn't work,

    make maintainer-clean
    ./autogen.sh
    ./configure
    make
