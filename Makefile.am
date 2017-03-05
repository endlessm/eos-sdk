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
AM_CPPFLAGS = -I$(builddir)/endless # needed for eosversion.h include to work in out of tree builds
AM_CFLAGS   = @STRICT_CFLAGS@

# Make sure to run Gtk-doc tests and build the documentation when doing
# 'make distcheck'
AM_DISTCHECK_CONFIGURE_FLAGS = \
	--enable-gtk-doc \
	--enable-gir-doc \
	--enable-js-doc \
	--enable-webhelper \
	$(NULL)
if EOS_ENABLE_COVERAGE
AM_DISTCHECK_CONFIGURE_FLAGS += --enable-coverage --with-coverage-dir=@EOS_COVERAGE_DIR@
endif EOS_ENABLE_COVERAGE

# Generated files that 'make clean' should erase
CLEANFILES =
DISTCLEANFILES =

# Other targets to add to
lib_LTLIBRARIES =

EOS_JS_COVERAGE_FILES =

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

# # # SUBSTITUTED FILES # # #
# These files need to be filled in with make variables

subst = $(SED) \
	-e 's,%libexecdir%,$(libexecdir),g' \
	$(NULL)

subst_files =
if ENABLE_WEBHELPER
subst_files += \
	webhelper/webhelper_private/config.js \
	$(NULL)
endif ENABLE_WEBHELPER

$(subst_files): %: %.in Makefile
	$(AM_V_GEN)$(MKDIR_P) $(@D) && \
	rm -f $@ $@.tmp && \
	$(subst) $< > $@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@

CLEANFILES += $(subst_files)
EXTRA_DIST += $(patsubst %,%.in,$(subst_files))

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

if ENABLE_WEBHELPER

webhelper_sources = \
	webhelper/webhelper.js \
	webhelper/webhelper2.js \
	$(NULL)

gjsmodulesdir = $(datadir)/gjs-1.0
webhelperdir = $(gjsmodulesdir)
webhelper_privatedir = $(webhelperdir)/webhelper_private
dist_webhelper_DATA = \
	$(webhelper_sources) \
	$(NULL)
dist_webhelper_private_DATA = \
	webhelper/webhelper_private/config.js \
	$(NULL)

EOS_JS_COVERAGE_FILES += $(webhelper_sources)

## Workaround for https://bugs.webkit.org/show_bug.cgi?id=116672
## When that is solved, we can eliminate this private library and go back to
## using pure Javascript in WebHelper.
lib_LTLIBRARIES += libwebhelper2private.la
libwebhelper2private_la_SOURCES = \
	webhelper/lib/wh2private.c \
	webhelper/lib/wh2private.h \
	$(NULL)
libwebhelper2private_la_CPPFLAGS = @WEBHELPER2_PRIVATE_CFLAGS@
libwebhelper2private_la_LIBADD = @WEBHELPER2_PRIVATE_LIBS@
libwebhelper2private_la_LDFLAGS = -avoid-version

webhelper2extensionsdir = $(libexecdir)/webhelper2
webhelper2extensions_LTLIBRARIES = wh2extension.la
wh2extension_la_SOURCES = \
	webhelper/webextensions/wh2extension.c \
	webhelper/webextensions/wh2jscutil.c \
	webhelper/webextensions/wh2jscutil.h \
	$(NULL)
wh2extension_la_CPPFLAGS = @WEBHELPER2_EXTENSION_CFLAGS@
wh2extension_la_LIBADD = @WEBHELPER2_EXTENSION_LIBS@
wh2extension_la_LDFLAGS = -module -avoid-version -no-undefined

endif ENABLE_WEBHELPER

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
	-I$(builddir)/endless \
	-DCOMPILING_EOS_SDK
# Endless_@EOS_SDK_API_VERSION@_gir_CFLAGS = $(INCLUDES)
Endless_@EOS_SDK_API_VERSION@_gir_LIBS = libendless-@EOS_SDK_API_VERSION@.la
Endless_@EOS_SDK_API_VERSION@_gir_FILES = $(introspection_sources)
Endless_@EOS_SDK_API_VERSION@_gir_EXPORT_PACKAGES = @EOS_SDK_API_NAME@
INTROSPECTION_GIRS += Endless-@EOS_SDK_API_VERSION@.gir

if ENABLE_WEBHELPER

WebHelper2Private-1.0.gir: libwebhelper2private.la
WebHelper2Private_1_0_gir_INCLUDES = GObject-2.0 GLib-2.0 WebKit2-4.0
WebHelper2Private_1_0_gir_SCANNERFLAGS = \
	--identifier-prefix=Wh2 \
	--symbol-prefix=wh2 \
	$(NULL)
WebHelper2Private_1_0_gir_LIBS = libwebhelper2private.la
WebHelper2Private_1_0_gir_FILES = $(libwebhelper2private_la_SOURCES)
INTROSPECTION_GIRS += WebHelper2Private-1.0.gir

endif ENABLE_WEBHELPER

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
nobase_dist_licenses_DATA = licenses/creativecommons/C/CC-BY-2.0.html \
	licenses/creativecommons/C/CC-BY-3.0.html \
	licenses/creativecommons/C/CC-BY-4.0.html \
	licenses/creativecommons/C/CC-BY-NC-2.0.html \
	licenses/creativecommons/C/CC-BY-NC-3.0.html \
	licenses/creativecommons/C/CC-BY-NC-SA-2.0.html \
	licenses/creativecommons/C/CC-BY-ND-2.0.html \
	licenses/creativecommons/C/CC-BY-ND-3.0.html \
	licenses/creativecommons/C/CC-BY-SA-2.0.html \
	licenses/creativecommons/C/CC-BY-SA-2.5.html \
	licenses/creativecommons/C/CC-BY-SA-3.0.html \
	licenses/creativecommons/C/CC-BY-SA-4.0.html \
	licenses/creativecommons/ar/CC-BY-2.0.html \
	licenses/creativecommons/ar/CC-BY-3.0.html \
	licenses/creativecommons/ar/CC-BY-4.0.html \
	licenses/creativecommons/ar/CC-BY-NC-2.0.html \
	licenses/creativecommons/ar/CC-BY-NC-3.0.html \
	licenses/creativecommons/ar/CC-BY-NC-SA-2.0.html \
	licenses/creativecommons/ar/CC-BY-ND-2.0.html \
	licenses/creativecommons/ar/CC-BY-ND-3.0.html \
	licenses/creativecommons/ar/CC-BY-SA-2.0.html \
	licenses/creativecommons/ar/CC-BY-SA-2.5.html \
	licenses/creativecommons/ar/CC-BY-SA-3.0.html \
	licenses/creativecommons/ar/CC-BY-SA-4.0.html \
	licenses/creativecommons/es/CC-BY-2.0.html \
	licenses/creativecommons/es/CC-BY-3.0.html \
	licenses/creativecommons/es/CC-BY-4.0.html \
	licenses/creativecommons/es/CC-BY-NC-2.0.html \
	licenses/creativecommons/es/CC-BY-NC-3.0.html \
	licenses/creativecommons/es/CC-BY-NC-SA-2.0.html \
	licenses/creativecommons/es/CC-BY-ND-2.0.html \
	licenses/creativecommons/es/CC-BY-ND-3.0.html \
	licenses/creativecommons/es/CC-BY-SA-2.0.html \
	licenses/creativecommons/es/CC-BY-SA-2.5.html \
	licenses/creativecommons/es/CC-BY-SA-3.0.html \
	licenses/creativecommons/es/CC-BY-SA-4.0.html \
	licenses/creativecommons/fr/CC-BY-2.0.html \
	licenses/creativecommons/fr/CC-BY-3.0.html \
	licenses/creativecommons/fr/CC-BY-4.0.html \
	licenses/creativecommons/fr/CC-BY-NC-2.0.html \
	licenses/creativecommons/fr/CC-BY-NC-3.0.html \
	licenses/creativecommons/fr/CC-BY-NC-SA-2.0.html \
	licenses/creativecommons/fr/CC-BY-ND-2.0.html \
	licenses/creativecommons/fr/CC-BY-ND-3.0.html \
	licenses/creativecommons/fr/CC-BY-SA-2.0.html \
	licenses/creativecommons/fr/CC-BY-SA-2.5.html \
	licenses/creativecommons/fr/CC-BY-SA-3.0.html \
	licenses/creativecommons/fr/CC-BY-SA-4.0.html \
	licenses/creativecommons/pt_BR/CC-BY-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-4.0.html \
	licenses/creativecommons/pt_BR/CC-BY-NC-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-NC-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-NC-SA-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-ND-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-ND-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-2.0.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-2.5.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-3.0.html \
	licenses/creativecommons/pt_BR/CC-BY-SA-4.0.html \
	licenses/creativecommons/legalcode/CC-BY-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-4.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-NC-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-NC-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-NC-SA-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-ND-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-ND-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-2.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-2.5-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-3.0-legalcode.html \
	licenses/creativecommons/legalcode/CC-BY-SA-4.0-legalcode.html \
	licenses/gnu/C/FDL-1.1.html \
	licenses/gnu/C/FDL-1.2.html \
	licenses/gnu/C/FDL-1.3.html \
	licenses/gnu/css/combo.css \
	licenses/gnu/css/layout.min.css \
	licenses/gnu/css/mini.css \
	licenses/gnu/css/print.min.css \
	licenses/gnu/images/dogear.png \
	licenses/gnu/images/fsf-logo-notext.png \
	licenses/gnu/images/heckert_gnu.small.png \
	licenses/gnu/images/topbanner.png \
	licenses/publicdomain/C/CC0-1.0.html \
	licenses/publicdomain/ar/CC0-1.0.html \
	licenses/publicdomain/es/CC0-1.0.html \
	licenses/publicdomain/fr/CC0-1.0.html \
	licenses/publicdomain/legalcode/CC0-1.0-legalcode.html \
	licenses/publicdomain/pt_BR/CC0-1.0.html \
	licenses/images/deed/cc-logo.jpg \
	licenses/images/deed/by.png \
	licenses/images/deed/deed-foot-green.png \
	licenses/images/deed/deed-foot-grey.png \
	licenses/images/deed/deed-foot-red.png \
	licenses/images/deed/deed-foot-yellow.png \
	licenses/images/deed/deed-head-green.png \
	licenses/images/deed/deed-head-grey.png \
	licenses/images/deed/deed-head-red.png \
	licenses/images/deed/deed-head-yellow.png \
	licenses/images/deed/devnations.png \
	licenses/images/deed/freedoms-header.png \
	licenses/images/deed/limitations-header.png \
	licenses/images/deed/logo_code.gif \
	licenses/images/deed/more.png \
	licenses/images/deed/nc-eu.png \
	licenses/images/deed/nc-gt.png \
	licenses/images/deed/nc-jp.png \
	licenses/images/deed/nc.png \
	licenses/images/deed/nd.png \
	licenses/images/deed/no-endorse.png \
	licenses/images/deed/noc.png \
	licenses/images/deed/nolaw-small.png \
	licenses/images/deed/nolaw.png \
	licenses/images/deed/notice.png \
	licenses/images/deed/remix.png \
	licenses/images/deed/sa.png \
	licenses/images/deed/sampling.png \
	licenses/images/deed/seal.png \
	licenses/images/deed/share.png \
	licenses/images/deed/src.png \
	licenses/images/deed/understanding-header.png \
	licenses/images/deed/usage-guidelines-small.png \
	licenses/images/deed/usage-guidelines.png \
	licenses/images/information.png \
	licenses/images/international/unported.png \
	licenses/includes/deed.js \
	licenses/includes/deeds.css \
	licenses/includes/deed3-ie.css \
	licenses/includes/deed3-print.css \
	licenses/includes/deed3-rtl.css \
	licenses/includes/deed3.css \
	licenses/includes/errata.js \
	licenses/includes/help.js \
	licenses/includes/jurisdictions.css \
	licenses/includes/popup.css \
	licenses/includes/total.css \
	licenses/includes/yahooapis/2.6.0/connection-min.js \
	licenses/includes/yahooapis/2.6.0/container-min.js \
	licenses/includes/yahooapis/2.6.0/container.css \
	licenses/includes/yahooapis/2.6.0/json-min.js \
	licenses/includes/yahooapis/2.6.0/sprite.png \
	licenses/includes/yahooapis/2.6.0/yahoo-dom-event.js \
	licenses/includes/zero.css \
	$(NULL)

# # # PURE JAVASCRIPT MODULE DOCUMENTATION # # #

if ENABLE_WEBHELPER
include $(top_srcdir)/docs/reference/webhelper/Makefile.am.inc
endif ENABLE_WEBHELPER

# # # GJS OVERRIDES # # #

include $(top_srcdir)/overrides/Makefile.am.inc

# # # SDK TOOLS # # #

include $(top_srcdir)/tools/Makefile.am.inc

# # # INSTALLED M4 MACROS # # #

m4dir = ${datadir}/aclocal
dist_m4_DATA = \
	m4/eos-coverage.m4 \
	m4/eos-i18n-extras.m4 \
	$(NULL)

# # # TESTS # # #

include $(top_srcdir)/test/Makefile.am.inc

# # # COVERAGE # # #
EOS_COVERAGE_BLACKLIST_PATTERNS = "*test/endless*"

@EOS_COVERAGE_RULES@

AM_JS_LOG_FLAGS += @EOS_JS_COVERAGE_LOG_FLAGS@
AM_CFLAGS += @EOS_C_COVERAGE_CFLAGS@
AM_LDFLAGS = @EOS_C_COVERAGE_LDFLAGS@

if ENABLE_WEBHELPER
libwebhelper2private_la_LDFLAGS += @EOS_C_COVERAGE_LDFLAGS@
wh2extension_la_LDFLAGS += @EOS_C_COVERAGE_LDFLAGS@
endif ENABLE_WEBHELPER

clean-local:: clean-coverage
