# Copyright 2013 Endless Mobile, Inc.

endless_public_installed_headers = endless/endless.h

endless_private_installed_headers = \
	endless/apiversion.h \
	endless/enums.h \
	endless/macros.h \
	endless/types.h

endless_library_sources = \
	endless/hello.c \
	endless/init.c

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
