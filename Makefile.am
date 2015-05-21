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
EXTRA_DIST = README.md @EOS_SDK_API_NAME@.pc.in
# As above, but files in dist_noinst_DATA also need to be built by make
dist_noinst_DATA =

# Compiler flags
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
AM_CFLAGS = @STRICT_CFLAGS@

# Make sure to run Gtk-doc tests and build the documentation when doing
# 'make distcheck'
DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc --enable-gir-doc --enable-js-doc

# Generated files that 'make clean' should erase
CLEANFILES =
DISTCLEANFILES =

# Other targets to add to
lib_LTLIBRARIES =

# Make sure that 'make dist' includes documentation
if CAN_MAKE_DIST
dist-hook::
else
dist-hook::
	@echo "***"
	@echo "*** You must configure with --enable-gtk-doc, --enable-gir-doc, and"
	@echo "*** --enable-js-doc to run make dist or make distcheck."
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

# # # WEBHELPER LIBRARY # # #

webhelper_sources = webhelper/webhelper.js

gjsmodulesdir = $(datadir)/gjs-1.0
webhelperdir = $(gjsmodulesdir)
dist_webhelper_DATA = \
	$(webhelper_sources) \
	$(NULL)

EOS_JS_COVERAGE_FILES = $(webhelper_sources)

# # # INTROSPECTION FILES # # #

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

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

# # # LICENSES DATA # # #

licensesdir = $(datadir)
nobase_dist_licenses_DATA = licenses/creativecommons/C/CC-BY-3.0.html \
	licenses/creativecommons/C/CC-BY-4.0.html \
	licenses/creativecommons/C/CC-BY-ND-2.0.html \
	licenses/creativecommons/C/CC-BY-ND-3.0.html \
	licenses/creativecommons/C/CC-BY-SA-3.0.html \
	licenses/creativecommons/C/CC-BY-SA-4.0.html \
	licenses/creativecommons/ar/CC-BY-3.0.html \
	licenses/creativecommons/ar/CC-BY-4.0.html \
	licenses/creativecommons/ar/CC-BY-ND-2.0.html \
	licenses/creativecommons/ar/CC-BY-ND-3.0.html \
	licenses/creativecommons/ar/CC-BY-SA-3.0.html \
	licenses/creativecommons/ar/CC-BY-SA-4.0.html \
	licenses/creativecommons/es/CC-BY-3.0.html \
	licenses/creativecommons/es/CC-BY-4.0.html \
	licenses/creativecommons/es/CC-BY-ND-2.0.html \
	licenses/creativecommons/es/CC-BY-ND-3.0.html \
	licenses/creativecommons/es/CC-BY-SA-3.0.html \
	licenses/creativecommons/es/CC-BY-SA-4.0.html \
	licenses/creativecommons/fr/CC-BY-3.0.html \
	licenses/creativecommons/fr/CC-BY-4.0.html \
	licenses/creativecommons/fr/CC-BY-ND-2.0.html \
	licenses/creativecommons/fr/CC-BY-ND-3.0.html \
	licenses/creativecommons/fr/CC-BY-SA-3.0.html \
	licenses/creativecommons/fr/CC-BY-SA-4.0.html \
	licenses/creativecommons/pt_BR/CC-BY-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-4.0.html \
	licenses/creativecommons/pt_BR/CC-BY-ND-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-ND-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-4.0.html \
	licenses/creativecommons/legalcode/CC-BY-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-4.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-ND-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-ND-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-4.0-legalcode.html \
	licenses/creativecommons/images/deed/cc-logo.jpg \
	licenses/creativecommons/images/deed/by.png \
	licenses/creativecommons/images/deed/deed-foot-green.png \
	licenses/creativecommons/images/deed/deed-foot-grey.png \
	licenses/creativecommons/images/deed/deed-foot-red.png \
	licenses/creativecommons/images/deed/deed-foot-yellow.png \
	licenses/creativecommons/images/deed/deed-head-green.png \
	licenses/creativecommons/images/deed/deed-head-grey.png \
	licenses/creativecommons/images/deed/deed-head-red.png \
	licenses/creativecommons/images/deed/deed-head-yellow.png \
	licenses/creativecommons/images/deed/devnations.png \
	licenses/creativecommons/images/deed/freedoms-header.png \
	licenses/creativecommons/images/deed/limitations-header.png \
	licenses/creativecommons/images/deed/logo_code.gif \
	licenses/creativecommons/images/deed/more.png \
	licenses/creativecommons/images/deed/nc-eu.png \
	licenses/creativecommons/images/deed/nc-gt.png \
	licenses/creativecommons/images/deed/nc-jp.png \
	licenses/creativecommons/images/deed/nc.png \
	licenses/creativecommons/images/deed/nd.png \
	licenses/creativecommons/images/deed/no-endorse.png \
	licenses/creativecommons/images/deed/noc.png \
	licenses/creativecommons/images/deed/nolaw-small.png \
	licenses/creativecommons/images/deed/nolaw.png \
	licenses/creativecommons/images/deed/notice.png \
	licenses/creativecommons/images/deed/remix.png \
	licenses/creativecommons/images/deed/sa.png \
	licenses/creativecommons/images/deed/sampling.png \
	licenses/creativecommons/images/deed/seal.png \
	licenses/creativecommons/images/deed/share.png \
	licenses/creativecommons/images/deed/src.png \
	licenses/creativecommons/images/deed/understanding-header.png \
	licenses/creativecommons/images/deed/usage-guidelines-small.png \
	licenses/creativecommons/images/deed/usage-guidelines.png \
	licenses/creativecommons/images/information.png \
	licenses/creativecommons/images/international/unported.png \
	licenses/creativecommons/includes/deed.js \
	licenses/creativecommons/includes/deeds.css \
	licenses/creativecommons/includes/deed3-ie.css \
	licenses/creativecommons/includes/deed3-print.css \
	licenses/creativecommons/includes/deed3-rtl.css \
	licenses/creativecommons/includes/deed3.css \
	licenses/creativecommons/includes/errata.js \
	licenses/creativecommons/includes/help.js \
	licenses/creativecommons/includes/jurisdictions.css \
	licenses/creativecommons/includes/popup.css \
	licenses/creativecommons/includes/total.css \
	licenses/creativecommons/includes/yahooapis/2.6.0/connection-min.js \
	licenses/creativecommons/includes/yahooapis/2.6.0/container-min.js \
	licenses/creativecommons/includes/yahooapis/2.6.0/container.css \
	licenses/creativecommons/includes/yahooapis/2.6.0/json-min.js \
	licenses/creativecommons/includes/yahooapis/2.6.0/sprite.png \
	licenses/creativecommons/includes/yahooapis/2.6.0/yahoo-dom-event.js \
	licenses/creativecommons/includes/zero.css \
	$(NULL)

# # # PURE JAVASCRIPT MODULE DOCUMENTATION # # #

include $(top_srcdir)/docs/reference/webhelper/Makefile.am.inc

# # # GJS OVERRIDES # # #

include $(top_srcdir)/overrides/Makefile.am.inc

# # # SDK TOOLS # # #

include $(top_srcdir)/tools/Makefile.am.inc

# # # INSTALLED M4 MACROS # # #

m4dir = ${datadir}/aclocal
m4_DATA = \
	m4/eos-coverage.m4 \
	$(NULL)

# # # TESTS # # #

include $(top_srcdir)/test/Makefile.am.inc

# # # COVERAGE # # #
EOS_COVERAGE_BLACKLIST_PATTERNS = "*test/endless*"

@EOS_COVERAGE_RULES@

AM_JS_LOG_FLAGS += @EOS_JS_COVERAGE_LOG_FLAGS@
AM_CFLAGS += @EOS_C_COVERAGE_CFLAGS@
AM_LDFLAGS = @EOS_C_COVERAGE_LDFLAGS@
