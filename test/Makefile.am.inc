# Copyright 2013 Endless Mobile, Inc.

TEST_FLAGS = @EOS_SDK_CFLAGS@ -I$(top_srcdir) -DCOMPILING_EOS_SDK
TEST_LIBS = @EOS_SDK_LIBS@ $(top_builddir)/libendless-@EOS_SDK_API_VERSION@.la

noinst_PROGRAMS = \
	test/endless/run-tests \
	test/smoke-tests/hello \
	$(NULL)

test_endless_run_tests_SOURCES = \
	test/endless/run-tests.c \
	test/endless/run-tests.h \
	test/endless/test-init.c \
	test/endless/test-hello.c \
	test/endless/test-application.c \
	test/endless/test-page-manager.c \
	test/endless/test-window.c \
	test/endless/test-flexy-grid.c \
	test/endless/test-custom-container.c \
	$(NULL)
test_endless_run_tests_CPPFLAGS = $(TEST_FLAGS)
test_endless_run_tests_LDADD = $(TEST_LIBS)

test_smoke_tests_hello_SOURCES = test/smoke-tests/hello.c
test_smoke_tests_hello_CPPFLAGS = $(TEST_FLAGS)
test_smoke_tests_hello_LDADD = $(TEST_LIBS)

credits_resource_files = \
	test/smoke-tests/images/test1.jpg \
	test/smoke-tests/images/test2.jpg \
	test/smoke-tests/images/test3.jpg \
	test/smoke-tests/images/attribution.json \
	$(NULL)
test/smoke-tests/images/credits.gresource: test/smoke-tests/images/credits.gresource.xml $(credits_resource_files)
	$(AM_V_GEN)$(GLIB_COMPILE_RESOURCES) --target=$@ --sourcedir=$(srcdir)/test/smoke-tests/images $<
dist_noinst_DATA += test/smoke-tests/images/credits.gresource
CLEANFILES += test/smoke-tests/images/credits.gresource
EXTRA_DIST += \
	test/smoke-tests/images/credits.gresource.xml \
	$(credits_resource_files) \
	$(NULL)

javascript_tests = \
	test/tools/eos-application-manifest/testInit.js \
	test/tools/testHtmlExtractor.js \
	test/webhelper/testLocal.js \
	test/webhelper/testTranslate.js \
	test/webhelper/testTranslate2.js \
	test/webhelper/testTranslate2Old.js \
	test/webhelper/testWebActions.js \
	test/webhelper/testWebActions2.js \
	test/webhelper/testWebActions2Old.js \
	test/webhelper/testUpdateFontSize.js \
	test/endless/testCustomContainer.js \
	test/endless/testTopbarNavButton.js \
	test/endless/testSearchBox.js \
	$(NULL)
EXTRA_DIST += \
	$(javascript_tests) \
	test/tools/test.html \
	test/utils.js \
	$(NULL)

# Run tests when running 'make check'
TESTS = \
	test/endless/run-tests \
	$(javascript_tests) \
	run_coverage.coverage \
	$(NULL)
JS_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(top_srcdir)/test/run-with-dbus --session $(SHELL) $(top_srcdir)/tap-driver.sh
JS_LOG_DRIVER_FLAGS = --comments
TEST_EXTENSIONS = .js .coverage
JS_LOG_COMPILER = jasmine
AM_JS_LOG_FLAGS = \
	--include-path=$(top_srcdir)/webhelper \
	--include-path=$(top_srcdir) \
	--tap \
	@JASMINE_REPORT_ARGUMENT@ \
	--no-config \
	$(NULL)
LOG_COMPILER = gtester
AM_LOG_FLAGS = -k --verbose
EXTRA_DIST += \
	test/run-with-dbus \
	test/test-bus.conf \
	$(NULL)
CLEANFILES += stderr.log

# Use locally built versions of Endless-0.gir and libraries; this may need to be
# changed to AM_TESTS_ENVIRONMENT in a future version of Automake
# Set XDG_CONFIG_HOME so as to avoid cluttering the user's actual config
# directory with junk from the tests
TESTS_ENVIRONMENT = \
	export GI_TYPELIB_PATH="$(top_builddir)$${GI_TYPELIB_PATH:+:$$GI_TYPELIB_PATH}"; \
	export LD_LIBRARY_PATH="$(top_builddir)/.libs$${LD_LIBRARY_PATH:+:$$LD_LIBRARY_PATH}"; \
	export XDG_CONFIG_HOME=`mktemp -d $${TMPDIR:-/tmp}/sdktestconfig.XXXXXXXX`; \
	export WEBHELPER_UNINSTALLED_EXTENSION_DIR="$(top_builddir)/.libs"; \
	export TOP_SRCDIR="$(abs_top_srcdir)"; \
	export BUILDDIR=".";
	$(NULL)
