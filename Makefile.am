## Process this file with automake to produce Makefile.in

# Copyright 2013 Endless Mobile, Inc.

## -----------
## Makefile.am
## -----------
## Please keep this file well-commented.

# Subdirectories where we also want to run make
# (Make sure that the library is built before the docs)
SUBDIRS = po . docs/reference/endless

# Our Autoconf macros are kept in the m4/ directory
ACLOCAL_AMFLAGS = -I m4

# Extra files to distribute in the tarball
EXTRA_DIST = @EOS_SDK_API_NAME@.pc.in

# Compiler flags
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AM_CFLAGS = @STRICT_CFLAGS@

# Make sure to run Gtk-doc tests and build the documentation when doing
# 'make distcheck'
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-gir-doc

# Generated files that 'make clean' should erase
CLEANFILES =
DISTCLEANFILES =

# Make sure that 'make dist' includes documentation
if CAN_MAKE_DIST
dist-hook:
else
dist-hook:
	@echo "***"
	@echo "*** You must configure with --enable-gtk-doc and --enable-gir-doc"
	@echo "*** to run make dist or make distcheck."
	@echo "***"
	@false
endif

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

# Open Endless Wikipedia SDK library
include $(top_srcdir)/wikipedia/Makefile.am.inc

# # # INTROSPECTION FILES # # #

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = \
	$(filter-out %-private.h, $(endless_library_sources)) \
	$(endless_public_installed_headers) \
	$(endless_private_installed_headers)

Endless-@EOS_SDK_API_VERSION@.gir: libendless-@EOS_SDK_API_VERSION@.la
Endless_@EOS_SDK_API_VERSION@_gir_INCLUDES = \
	GObject-2.0 GLib-2.0 Gio-2.0 Gtk-3.0
Endless_@EOS_SDK_API_VERSION@_gir_SCANNERFLAGS = \
	--identifier-prefix=Eos \
	--symbol-prefix=eos \
	-DCOMPILING_EOS_SDK
# Endless_@EOS_SDK_API_VERSION@_gir_CFLAGS = $(INCLUDES)
Endless_@EOS_SDK_API_VERSION@_gir_LIBS = libendless-@EOS_SDK_API_VERSION@.la
Endless_@EOS_SDK_API_VERSION@_gir_FILES = $(introspection_sources)
Endless_@EOS_SDK_API_VERSION@_gir_EXPORT_PACKAGES = @EOS_SDK_API_NAME@
INTROSPECTION_GIRS += Endless-@EOS_SDK_API_VERSION@.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

# # # GOBJECT INTROSPECTION DOCUMENTATION # # #

if ENABLE_GIR_DOC

docs/reference/endless-js/index.page: $(INTROSPECTION_GIRS)
	$(MKDIR_P) $(@D)
	g-ir-doc-tool -o $(@D) -l gjs $<

docs/reference/endless-js/html/index.html: docs/reference/endless-js/index.page
	$(MKDIR_P) $(@D)
	yelp-build html -o $(@D) $(<D)

all-local: docs/reference/endless-js/html/index.html

EXTRA_DIST += \
	docs/reference/endless-js/html/*.html \
	docs/reference/endless-js/html/*.css \
	docs/reference/endless-js/html/*.js

CLEANFILES += \
	docs/reference/endless-js/*.page \
	docs/reference/endless-js/html/*.html \
	docs/reference/endless-js/html/*.css \
	docs/reference/endless-js/html/*.js
endif

# # # SECONDARY GRESOURCES # # #

wikipedia_resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir)/data --generate-dependencies $(srcdir)/data/eos-wikipedia-domain.gresource.xml)
data/eos-wikipedia-domain.gresource: data/eos-wikipedia-domain.gresource.xml $(wikipedia_resource_files)
	$(AM_V_GEN) $(MKDIR_P) $(builddir)/data && \
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)/data  $<

wikipediadatadir = $(datadir)/eos-wikipedia-domain
wikipediadata_DATA = data/eos-wikipedia-domain.gresource
EXTRA_DIST += \
	$(wikipedia_resource_files) \
	data/eos-wikipedia-domain.gresource.xml \
	$(NULL)
CLEANFILES += data/eos-wikipedia-domain.gresource

# # # GJS OVERRIDES # # #

gjsoverridedir = ${datadir}/gjs-1.0/overrides

# Put override files here for custom js API outside of normal introspection
dist_gjsoverride_DATA = 		\
	overrides/Endless.js

# # # TESTS # # #

include $(top_srcdir)/test/Makefile.am

# Run tests when running 'make check'
TESTS_ENVIRONMENT = gtester -k -o test-results.xml --verbose
TESTS = test/run-tests

CLEANFILES += test-results.xml
