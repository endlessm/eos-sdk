# Copyright 2013 Endless Mobile, Inc.

noinst_PROGRAMS = \
	test/run-tests \
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
	$(NULL)
test_run_tests_CPPFLAGS = $(TEST_FLAGS)
test_run_tests_LDADD = $(TEST_LIBS)

test_smoke_tests_hello_SOURCES = test/smoke-tests/hello.c
test_smoke_tests_hello_CPPFLAGS = $(TEST_FLAGS)
test_smoke_tests_hello_LDADD = $(TEST_LIBS)

javascript_tests = \
	test/tools/eos-run-test/sanitycheck.js \
	$(NULL)
EXTRA_DIST += $(javascript_tests)
