## Process this file with automake to produce Makefile.in

# Copyright 2013 Endless Mobile, Inc.

## -----------
## Makefile.am
## -----------
## Please keep this file well-commented.

# Subdirectories where we also want to run make
SUBDIRS = po

# Our Autoconf macros are kept in the m4/ directory
ACLOCAL_AMFLAGS = -I m4

# Extra files to distribute in the tarball
EXTRA_DIST = @EOS_SDK_API_NAME@.pc.in

# Compiler flags
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AM_CFLAGS = @STRICT_CFLAGS@

# Generated files that 'make clean' should erase
CLEANFILES =
DISTCLEANFILES =

# # # LIBRARY # # #

# Main Open Endless SDK library
include $(top_srcdir)/endless/Makefile.am

# Public header files
endlessincludedir = $(includedir)/@EOS_SDK_API_NAME@
nobase_endlessinclude_HEADERS = \
	$(endless_public_installed_headers) \
	$(endless_private_installed_headers)

# Pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @EOS_SDK_API_NAME@.pc
DISTCLEANFILES += @EOS_SDK_API_NAME@.pc

# # # TESTS # # #

include $(top_srcdir)/test/Makefile.am

# Run tests when running 'make check'
TESTS_ENVIRONMENT = gtester -k -o test-results.xml
TESTS = test/run-tests

CLEANFILES += test-results.xml
