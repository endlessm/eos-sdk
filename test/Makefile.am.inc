# Copyright 2013 Endless Mobile, Inc.

TEST_FLAGS = @EOS_SDK_CFLAGS@ -I$(top_srcdir) -DCOMPILING_EOS_SDK
TEST_LIBS = @EOS_SDK_LIBS@ $(top_builddir)/libendless-@EOS_SDK_API_VERSION@.la
ENDLESS_TESTS_DIRECTORY = $(top_srcdir)/test

noinst_PROGRAMS = \
	test/endless/run-tests \
	test/smoke-tests/hello \
	test/demos/flexy-grid

# This variable will be updated in Makefile subdirs (test/endless/Makefile.am.inc)
javascript_tests = \
	test/tools/eos-application-manifest/testInit.js \
	test/webhelper/testTranslate.js \
	test/webhelper/testWebActions.js \
	test/webhelper/testUpdateFontSize.js \
	test/wikipedia/models/testCategoryModel.js \
	test/wikipedia/models/testArticleModel.js \
	test/wikipedia/models/testDomainWikiModel.js \
	$(NULL)
EXTRA_DIST += $(javascript_tests)

include test/endless/Makefile.am.inc
include test/demos/Makefile.am.inc
include test/smoke-tests/Makefile.am.inc

# Run tests when running 'make check'
TESTS = \
	test/endless/run-tests \
	$(javascript_tests) \
	$(NULL)
TEST_EXTENSIONS =
AM_JS_LOG_FLAGS = \
	--include-path=$(top_srcdir)/webhelper \
	--include-path=$(top_srcdir) \
	$(NULL)
LOG_COMPILER = gtester
AM_LOG_FLAGS = -k --verbose

JASMINE_SUBMODULE_PATH = $(top_srcdir)/test/jasmine
include test/jasmine/Makefile-jasmine.am.inc

# Use locally built versions of Endless-0.gir and libraries; this may need to be
# changed to AM_TESTS_ENVIRONMENT in a future version of Automake
# Set XDG_CONFIG_HOME so as to avoid cluttering the user's actual config
# directory with junk from the tests
TESTS_ENVIRONMENT = \
	export GI_TYPELIB_PATH="$(top_builddir)$${GI_TYPELIB_PATH:+:$$GI_TYPELIB_PATH}"; \
	export LD_LIBRARY_PATH="$(top_builddir)/.libs$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}"; \
	export XDG_CONFIG_HOME=`mktemp -d $${TMPDIR:-/tmp}/sdktestconfig.XXXXXXXX`; \
	$(NULL)
