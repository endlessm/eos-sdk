dnl Process this file with autoconf to produce configure

dnl ------------
dnl configure.ac
dnl ------------
dnl Please keep this file well-commented. Autotools involve a lot of magical
dnl incantations, and it is easy to mess things up if you don't know what you
dnl are doing.

# Initialization
# --------------
# Initialize Autoconf: package name, version
AC_INIT([Open Endless SDK], [0.0])
# Verify that the source directory can be found
dnl AC_CONFIG_SRCDIR([src/hello.c])
# Initialize Automake: enable all warnings and do not insist on GNU standards
AM_INIT_AUTOMAKE([-Wall foreign])
# Avoid spewing garbage over the terminal ('make V=1' to see the garbage)
AM_SILENT_RULES([yes])
# Initialize Libtool; don't build static libraries
LT_INIT([disable-static])
# Keep Autotools macros local to this source tree
AC_CONFIG_MACRO_DIR([m4])

# Variables to define
# -------------------
# API name and version for pkg-config; update when breaking API/ABI.
# This version number is different from both the package version number and the
# shared library versioning number.
EOS_SDK_API_VERSION=0
AC_SUBST(EOS_SDK_API_VERSION)
EOS_SDK_API_NAME="endless-$EOS_SDK_API_VERSION"
AC_SUBST(EOS_SDK_API_NAME)

# Shared library versioning; only update immediately before a public release.
# See http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
EOS_SDK_CURRENT=0
EOS_SDK_REVISION=0
EOS_SDK_AGE=0
EOS_SDK_LIBTOOL_VERSION_INFO="$EOS_SDK_CURRENT:$EOS_SDK_REVISION:$EOS_SDK_AGE"
AC_SUBST(EOS_SDK_LIBTOOL_VERSION_INFO)

# Required versions of libraries
# Update these whenever you use a function that requires a certain API version
GLIB_REQUIRED_VERSION=2.0
GTK_REQUIRED_VERSION=3.0
AC_SUBST(GLIB_REQUIRED_VERSION)
AC_SUBST(GTK_REQUIRED_VERSION)

# Gettext package name
GETTEXT_PACKAGE=$PACKAGE
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"],
	[Package name for Gettext])

# Required build tools
# --------------------
# C compiler
AC_PROG_CC
# Library configuration tool; 0.24 required for querying install path
PKG_PROG_PKG_CONFIG([0.24])
# Gettext
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])

# Required libraries
# ------------------
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= $GLIB_REQUIRED_VERSION gobject-2.0 gio-2.0])
# PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= $GTK_REQUIRED_VERSION])

# Output
# ------
# List files here that the configure script should output
AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
	$EOS_SDK_API_NAME.pc
])
AC_CONFIG_HEADERS([config.h])  dnl Header with system-dependent #defines
# Do the output
AC_OUTPUT

