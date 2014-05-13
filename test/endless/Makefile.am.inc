# Copyright 2013 Endless Mobile, Inc.

test_endless_run_tests_SOURCES = \
	$(ENDLESS_TESTS_DIRECTORY)/endless/run-tests.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/run-tests.h \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-init.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-hello.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-application.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-page-manager.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-window.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-action-menu.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-action-button.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-flexy-grid.c \
	$(ENDLESS_TESTS_DIRECTORY)/endless/test-custom-container.c \
	$(NULL)
test_endless_run_tests_CPPFLAGS = $(TEST_FLAGS)
test_endless_run_tests_LDADD = $(TEST_LIBS)

javascript_tests += \
	test/endless/testCustomContainer.js \
	$(NULL)
