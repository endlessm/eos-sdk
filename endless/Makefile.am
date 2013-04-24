# Copyright 2013 Endless Mobile, Inc.

endless_public_installed_headers = endless/endless.h

endless_private_installed_headers = \
	endless/eosversion.h \
	endless/eosapplication.h \
	endless/eosenums.h \
	endless/eosmacros.h \
	endless/eostypes.h \
	endless/eoswindow.h

endless_library_sources = \
	endless/eosapplication.c \
	endless/eoshello.c \
	endless/eosinit.c endless/eosinit-private.h \
	endless/eoswindow.c

# Endless GUI library
lib_LTLIBRARIES = libendless-@EOS_SDK_API_VERSION@.la
libendless_@EOS_SDK_API_VERSION@_la_SOURCES = \
	$(endless_public_installed_headers) \
	$(endless_private_installed_headers) \
	$(endless_library_sources)
libendless_@EOS_SDK_API_VERSION@_la_CPPFLAGS = \
	@EOS_SDK_CFLAGS@ \
	-DCOMPILING_EOS_SDK
libendless_@EOS_SDK_API_VERSION@_la_CFLAGS = $(AM_CFLAGS)
libendless_@EOS_SDK_API_VERSION@_la_LIBADD = @EOS_SDK_LIBS@
libendless_@EOS_SDK_API_VERSION@_la_LDFLAGS = \
	-version-info @EOS_SDK_LT_VERSION_INFO@ \
	-export-symbols-regex "^eos_"
