## Process this file with automake to produce Makefile.in

## -----------
## Makefile.am
## -----------
## Please keep this file well-commented.

# Our Autoconf macros are kept in the m4/ directory
ACLOCAL_AMFLAGS = -I m4

# Extra files to distribute in the tarball
EXTRA_DIST = @EOS_SDK_API_NAME@.pc.in

# # # LIBRARY # # #

# Main Open Endless SDK library
lib_LTLIBRARIES = libendless.la
libendless_la_SOURCES = \
	src/endless/hello.c \
	src/endless/endless.h
libendless_la_CPPFLAGS = @GLIB_CFLAGS@
libendless_la_LIBADD = @GLIB_LIBS@
libendless_la_LDFLAGS = \
	-version-info @EOS_SDK_LIBTOOL_VERSION_INFO@ \
	-export-symbols-regex "^eos_"

# Public header files
endlessincludedir = $(includedir)/@EOS_SDK_API_NAME@/endless
endlessinclude_HEADERS = src/endless/endless.h

# Pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @EOS_SDK_API_NAME@.pc

# # # MISCELLANEOUS # # #

# Files to clean
DISTCLEANFILES = @EOS_SDK_API_NAME@.pc

# Git ignore magic
MAINTAINERCLEANFILES = \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL)
-include $(top_srcdir)/git.mk
