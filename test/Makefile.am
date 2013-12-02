# Copyright 2013 Endless Mobile, Inc.

noinst_PROGRAMS = \
	test/run-tests \
	test/flexy-grid \
	test/smoke-tests/hello

TEST_FLAGS = @EOS_SDK_CFLAGS@ -I$(top_srcdir) -DCOMPILING_EOS_SDK
TEST_LIBS = @EOS_SDK_LIBS@ $(top_builddir)/libendless-@EOS_SDK_API_VERSION@.la

test_run_tests_SOURCES = \
	test/run-tests.c test/run-tests.h \
	test/test-init.c \
	test/test-hello.c \
	test/test-application.c \
	test/test-page-manager.c \
	test/test-splash-page-manager.c \
	test/test-window.c \
	test/test-action-menu.c \
	test/test-action-button.c \
	test/test-flexy-grid.c \
	$(NULL)
test_run_tests_CPPFLAGS = $(TEST_FLAGS)
test_run_tests_LDADD = $(TEST_LIBS)

test_smoke_tests_hello_SOURCES = test/smoke-tests/hello.c
test_smoke_tests_hello_CPPFLAGS = $(TEST_FLAGS)
test_smoke_tests_hello_LDADD = $(TEST_LIBS)

test_flexy_grid_SOURCES = test/flexy-grid.c
test_flexy_grid_CPPFLAGS = $(TEST_FLAGS)
test_flexy_grid_LDADD = $(TEST_LIBS)

javascript_tests = \
	test/tools/eos-run-test/sanitycheck.js \
	test/webhelper/testTranslate.js \
	test/webhelper/testWebActions.js \
	test/wikipedia/models/testCategoryModel.js \
	test/wikipedia/models/testArticleModel.js \
	test/wikipedia/models/testDomainWikiModel.js \
	$(NULL)
EXTRA_DIST += $(javascript_tests)

# Run tests when running 'make check'
TESTS = \
	test/run-tests \
	$(javascript_tests) \
	$(NULL)
TEST_EXTENSIONS = .js
JS_LOG_COMPILER = tools/eos-run-test
AM_JS_LOG_FLAGS = \
	--include-path=$(top_srcdir)/webhelper \
	--include-path=$(top_srcdir) \
	$(NULL)
LOG_COMPILER = gtester
AM_LOG_FLAGS = -k --verbose

# Use locally built versions of Endless-0.gir and libraries; this may need to be
# changed to AM_TESTS_ENVIRONMENT in a future version of Automake
# Set XDG_CONFIG_HOME so as to avoid cluttering the user's actual config
# directory with junk from the tests
TESTS_ENVIRONMENT = \
	export GI_TYPELIB_PATH="$(top_builddir)$${GI_TYPELIB_PATH:+:$$GI_TYPELIB_PATH}"; \
	export LD_LIBRARY_PATH="$(top_builddir)/.libs$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}"; \
	export XDG_CONFIG_HOME=`mktemp -d $${TMPDIR:-/tmp}/sdktestconfig.XXXXXXXX`; \
	$(NULL)
