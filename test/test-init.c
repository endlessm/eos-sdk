/* Copyright 2013 Endless Mobile, Inc. */

#include <glib.h>
#include <endless/endless.h>
#include "endless/eosinit-private.h"

#include "run-tests.h"

static void
test_constructor_called (void)
{
  g_assert (eos_is_inited ());
}

void
add_init_tests (void)
{
  g_test_add_func ("/init/constructor-called", test_constructor_called);
}
