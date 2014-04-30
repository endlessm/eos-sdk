# Copyright 2013 Endless Mobile, Inc.

# Generate dependencies for make rule from XML resource file description
resource_files = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=$(srcdir)/data \
	--generate-dependencies $(srcdir)/data/eos-sdk.gresource.xml)

# Generated sources for GResource file
endless/eosresource-private.h: data/eos-sdk.gresource.xml $(resource_files)
	$(AM_V_GEN) $(MKDIR_P) $(builddir)/data && \
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)/data \
		--generate-header --internal $<
endless/eosresource.c: data/eos-sdk.gresource.xml $(resource_files)
	$(AM_V_GEN) $(MKDIR_P) $(builddir)/data && \
	$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)/data \
		--generate-source --internal $<

BUILT_SOURCES = \
	endless/eosresource-private.h \
	endless/eosresource.c \
	$(NULL)
EXTRA_DIST += \
	$(resource_files) \
	data/eos-sdk.gresource.xml

endless_public_installed_headers = endless/endless.h

endless_private_installed_headers = \
	endless/eosversion.h \
	endless/eosapplication.h \
	endless/eoscustomcontainer.h \
	endless/eosenums.h \
	endless/eosmacros.h \
	endless/eospagemanager.h \
	endless/eostypes.h \
	endless/eoswindow.h \
	endless/eosflexygrid.h

endless_library_sources = \
	endless/eosapplication.c \
	endless/eoscustomcontainer.c \
	endless/eoshello.c \
	endless/eosinit.c endless/eosinit-private.h \
	endless/eospagemanager.c \
	endless/eosresource.c endless/eosresource-private.h \
	endless/eostopbar.c endless/eostopbar-private.h \
	endless/eoswindow.c \
	endless/eosflexygrid.c endless/eosflexygridcell.c endless/eosflexygrid-private.h

# Endless GUI library
lib_LTLIBRARIES = libendless-@EOS_SDK_API_VERSION@.la
libendless_@EOS_SDK_API_VERSION@_la_SOURCES = \
	$(endless_public_installed_headers) \
	$(endless_private_installed_headers) \
	$(endless_library_sources)
# Defining G_LOG_DOMAIN allows you to run any program with
# G_MESSAGES_DEBUG=EndlessSDK ./myprogram
# and turn on debug messages
libendless_@EOS_SDK_API_VERSION@_la_CPPFLAGS = \
	@EOS_SDK_CFLAGS@ \
	-DG_LOG_DOMAIN=\"EndlessSDK\" \
	-DCOMPILING_EOS_SDK \
	-DDATADIR=\""$(datadir)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\"
libendless_@EOS_SDK_API_VERSION@_la_CFLAGS = $(AM_CFLAGS)
libendless_@EOS_SDK_API_VERSION@_la_LIBADD = @EOS_SDK_LIBS@

# AM_LDFLAGS won't be applied automatically where we have an override
# so we need to add EOS_C_COVERAGE_LDFLAGS here
libendless_@EOS_SDK_API_VERSION@_la_LDFLAGS = \
	-version-info @EOS_SDK_LT_VERSION_INFO@ \
	-export-symbols-regex "^eos_" \
	@EOS_C_COVERAGE_LDFLAGS@
