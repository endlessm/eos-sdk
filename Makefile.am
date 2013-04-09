## Process this file with automake to produce Makefile.in

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

# # # LIBRARY # # #

# Main Open Endless SDK library
lib_LTLIBRARIES = libendless.la
libendless_la_SOURCES = \
	src/endless/init.c src/endless/init.h \
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

# # # TESTS # # #

noinst_PROGRAMS = \
	test/run-tests \
	test/smoke-tests/hello

test_run_tests_SOURCES = \
	test/run-tests.c \
	test/test-hello.c
test_run_tests_CPPFLAGS = @GLIB_CFLAGS@ -I$(top_srcdir)/src
test_run_tests_LDADD = @GLIB_LIBS@ $(top_builddir)/libendless.la

test_smoke_tests_hello_SOURCES = test/smoke-tests/hello.c
test_smoke_tests_hello_CPPFLAGS = @GLIB_CFLAGS@ -I$(top_srcdir)/src
test_smoke_tests_hello_LDADD = @GLIB_LIBS@ $(top_builddir)/libendless.la

# Run tests when running 'make check'
TESTS_ENVIRONMENT = gtester -k -o test-results.xml
TESTS = test/run-tests

# # # MISCELLANEOUS # # #

# Files to clean
DISTCLEANFILES = @EOS_SDK_API_NAME@.pc

# Git ignore magic
GITIGNOREFILES = m4  # remove this when adding custom macros to m4/
GITIGNORE_MAINTAINERCLEANFILES_GETTEXT = \
	ABOUT-NLS \
	config.rpath \
	po/Makevars.template po/Rules-quot po/boldquot.sed po/en@boldquot.header \
	po/en@quot.header po/insert-header.sin po/quot.sed po/remove-potcdate.sed \
	po/remove-potcdate.sin po/stamp-po
GITIGNORE_MAINTAINERCLEANFILES_M4_GETTEXT = \
	codeset.m4 fcntl-o.m4 gettext.m4 glibc21.m4 glibc2.m4 iconv.m4 intdiv0.m4 \
	intldir.m4 intl.m4 intlmacosx.m4 intmax.m4 inttypes_h.m4 inttypes-pri.m4 \
	lcmessage.m4 lib-ld.m4 lib-link.m4 lib-prefix.m4 lock.m4 longlong.m4 \
	nls.m4 po.m4 printf-posix.m4 progtest.m4 size_max.m4 stdint_h.m4 \
	threadlib.m4 uintmax_t.m4 visibility.m4 wchar_t.m4 wint_t.m4 xsize.m4
MAINTAINERCLEANFILES = \
	$(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
	$(GITIGNORE_MAINTAINERCLEANFILES_GETTEXT) \
	$(GITIGNORE_MAINTAINERCLEANFILES_M4_GETTEXT)
-include $(top_srcdir)/git.mk
