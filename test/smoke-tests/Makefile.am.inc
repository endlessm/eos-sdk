# Copyright 2013 Endless Mobile, Inc.

test_smoke_tests_hello_SOURCES = $(ENDLESS_TESTS_DIRECTORY)/smoke-tests/hello.c
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
dist_noinst_DATA = test/smoke-tests/images/credits.gresource
CLEANFILES += test/smoke-tests/images/credits.gresource
EXTRA_DIST += \
	test/smoke-tests/images/credits.gresource.xml \
	$(credits_resource_files) \
	$(NULL)
