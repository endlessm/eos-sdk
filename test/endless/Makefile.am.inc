# Copyright 2013 Endless Mobile, Inc.

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

javascript_tests += \
	test/endless/testCustomContainer.js \
	test/endless/testTopbarNavButton.js \
	$(NULL)
