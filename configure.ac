dnl Process this file with autoconf to produce configure

# Copyright 2013 Endless Mobile, Inc.

dnl ------------
dnl configure.ac
dnl ------------
dnl Please keep this file well-commented. Autotools involve a lot of magical
dnl incantations, and it is easy to mess things up if you don't know what you
dnl are doing.

# Version numbers
# ---------------
# API name and version for pkg-config; increment when breaking API/ABI.
# This version number is also the major version number.
m4_define([_EOS_SDK_API_VERSION_MACRO], [0])

# Minor and micro versions: increment micro version when making a release. Minor
# version is even for a stable release and odd for a development release.
# When making any release, if the API changes, set the interface age to 0.
m4_define([_EOS_SDK_MINOR_VERSION_MACRO], [0])
m4_define([_EOS_SDK_MICRO_VERSION_MACRO], [0])
m4_define([_EOS_SDK_INTERFACE_AGE_MACRO], [0])

# Full version, for use in AC_INIT
m4_define([_EOS_SDK_VERSION_MACRO],
	[_EOS_SDK_API_VERSION_MACRO._EOS_SDK_MINOR_VERSION_MACRO._EOS_SDK_MICRO_VERSION_MACRO])

# Initialization
# --------------
# Initialize Autoconf: package name, version, bug report address, tarball name,
# website
AC_INIT([Open Endless SDK], [_EOS_SDK_VERSION_MACRO],
	[], [eos-sdk], [http://endlessm.com])
# Verify that the source directory can be found
dnl AC_CONFIG_SRCDIR([src/hello.c])
# Initialize Automake: enable all warnings and do not insist on GNU standards
# Automake >= 1.11 required for silent rules
# no-portability suppresses warnings about syntax specific to GNU make
AM_INIT_AUTOMAKE([-Wall -Wno-portability foreign 1.11])
# Avoid spewing garbage over the terminal ('make V=1' to see the garbage)
AM_SILENT_RULES([yes])
# Initialize Libtool; don't build static libraries
AM_PROG_AR  dnl must call before LT_INIT
LT_INIT([disable-static])
# Keep Autotools macros local to this source tree
AC_CONFIG_MACRO_DIR([m4])

AC_CACHE_SAVE

# Variables to define
# -------------------
# Version numbers and macros
EOS_SDK_API_VERSION=_EOS_SDK_API_VERSION_MACRO
AC_SUBST(EOS_SDK_API_VERSION)
AC_DEFINE([EOS_SDK_MAJOR_VERSION], [_EOS_SDK_API_VERSION_MACRO], [Major (API) version])
EOS_SDK_API_NAME="endless-$EOS_SDK_API_VERSION"
AC_SUBST(EOS_SDK_API_NAME)
EOS_SDK_MINOR_VERSION=_EOS_SDK_MINOR_VERSION_MACRO
EOS_SDK_MICRO_VERSION=_EOS_SDK_MICRO_VERSION_MACRO
EOS_SDK_INTERFACE_AGE=_EOS_SDK_INTERFACE_AGE_MACRO
AC_DEFINE([EOS_SDK_MINOR_VERSION], [_EOS_SDK_MINOR_VERSION_MACRO], [Minor version])
AC_DEFINE([EOS_SDK_MICRO_VERSION], [_EOS_SDK_MICRO_VERSION_MACRO], [Micro version])
AC_DEFINE([EOS_SDK_VERSION], [_EOS_SDK_VERSION_MACRO], [Full version number])

# Shared library versioning; calculated automatically.
# See http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
EOS_SDK_LT_CURRENT=m4_eval(100 * _EOS_SDK_MINOR_VERSION_MACRO + _EOS_SDK_MICRO_VERSION_MACRO - _EOS_SDK_INTERFACE_AGE_MACRO)
EOS_SDK_LT_REVISION=$EOS_SDK_INTERFACE_AGE
EOS_SDK_LT_AGE=$EOS_SDK_LT_CURRENT
EOS_SDK_LT_VERSION_INFO="$EOS_SDK_LT_CURRENT:$EOS_SDK_LT_REVISION:$EOS_SDK_LT_AGE"
AC_SUBST(EOS_SDK_LT_VERSION_INFO)

# Required versions of libraries
# Update these whenever you use a function that requires a certain API version
GLIB_REQUIREMENT="glib-2.0 >= 2.36"
GOBJECT_REQUIREMENT="gobject-2.0"
GIO_REQUIREMENT="gio-2.0"
GTK_REQUIREMENT="gtk+-3.0 >= 3.4"
# These go into the pkg-config file as Requires: and Requires.private:
# (Generally, use Requires.private: instead of Requires:)
EOS_REQUIRED_MODULES=
EOS_REQUIRED_MODULES_PRIVATE="$GLIB_REQUIREMENT $GOBJECT_REQUIREMENT $GIO_REQUIREMENT $GTK_REQUIREMENT"
AC_SUBST(EOS_REQUIRED_MODULES)
AC_SUBST(EOS_REQUIRED_MODULES_PRIVATE)

# Gettext package name
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"],
	[Package name for Gettext])
# Detect which languages are available
AS_ALL_LINGUAS

# Required build tools
# --------------------
# C compiler
AC_PROG_CC
# Make sure the C compiler supports per-target CFLAGS
AM_PROG_CC_C_O
# Library configuration tool
PKG_PROG_PKG_CONFIG
# Gettext
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])
# Gtk-doc; 0.18 required for Markdown parsing
GTK_DOC_CHECK([1.18], [--flavour no-tmpl])
# GObject Introspection
GOBJECT_INTROSPECTION_REQUIRE([1.30])

AC_CACHE_SAVE

# Check that the compiler supports C99, and enable it in our CFLAGS
AS_COMPILER_FLAGS(C99_CFLAGS, "-std=c99")
C99_CFLAGS=${C99_CFLAGS#*  }
CFLAGS="$CFLAGS $C99_CFLAGS"

# Configure options
# -----------------
# --enable-strict-flags: Compile with strict compiler flags. Done automatically
# during 'make distcheck'.
AC_ARG_ENABLE([strict-flags],
	[AS_HELP_STRING([--enable-strict-flags=@<:@no/yes/error@:>@],
		[Use strict compiler flags @<:@default=no@:>@])],
	[],
	[enable_strict_flags=no])
# Emmanuele's list of flags
STRICT_COMPILER_FLAGS="$STRICT_COMPILER_FLAGS
	-Wall
	-Wcast-align
	-Wuninitialized
	-Wno-strict-aliasing
	-Werror=pointer-arith
	-Werror=missing-declarations
	-Werror=redundant-decls
	-Werror=empty-body
	-Werror=format
	-Werror=format-security
	-Werror=format-nonliteral
	-Werror=init-self"
AS_CASE([$enable_strict_flags],
	[yes],
		[AS_COMPILER_FLAGS([STRICT_CFLAGS], [$STRICT_COMPILER_FLAGS])],
	[no],
		[],
        [error],
        [
        	STRICT_COMPILER_FLAGS="$STRICT_COMPILER_FLAGS -Werror"
			AS_COMPILER_FLAGS([STRICT_CFLAGS], [$STRICT_COMPILER_FLAGS])
        ],
	[AC_MSG_ERROR([Invalid option for --enable-strict-flags])])
dnl Strip leading spaces
STRICT_CFLAGS=${STRICT_CFLAGS#*  }
AC_SUBST(STRICT_CFLAGS)

# Required libraries
# ------------------
PKG_CHECK_MODULES([EOS_SDK], [
	$EOS_REQUIRED_MODULES
	$EOS_REQUIRED_MODULES_PRIVATE])

AC_CACHE_SAVE

# Output
# ------
# List files here that the configure script should output
AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
	docs/reference/endless/Makefile
	docs/reference/endless/version.xml
	$EOS_SDK_API_NAME.pc
])
AC_CONFIG_HEADERS([config.h])  dnl Header with system-dependent #defines
# Do the output
AC_OUTPUT

