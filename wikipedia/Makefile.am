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

# Generated files that 'make clean' should erase
CLEANFILES =
DISTCLEANFILES =
EXTRA_DIST =

# # # BUILD RULES # # #

resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir)/data --generate-dependencies $(srcdir)/data/endless_brazil.gresource.xml)
data/endless_brazil.gresource: data/endless_brazil.gresource.xml $(resource_files)
	$(AM_V_GEN) $(MKDIR_P) $(builddir)/data && \
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)/data  $<
CLEANFILES += data/endless_brazil.gresource

eos-brazil: Makefile
	echo "gjs $(pkgdatadir)/src/endless_brazil.js" >$@
CLEANFILES += eos-brazil

# # # INSTALL RULES # # #

js_sources = \
	src/endless_brazil.js \
	src/endless_wikipedia/EndlessWikipedia.js \
	src/endless_wikipedia/PrebuiltWikipediaApplication.js \
	$(NULL)
nobase_pkgdata_DATA = \
	data/endless_brazil.gresource \
	$(js_sources) \
	$(NULL)
bin_SCRIPTS = eos-brazil

EXTRA_DIST += \
	$(js_sources) \
	$(resource_files) \
	data/endless_brazil.gresource.xml \
	$(NULL)

# # # TESTS # # #
