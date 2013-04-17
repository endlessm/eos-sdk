/* Copyright 2013 Endless Mobile, Inc. */

#include <glib-object.h>
#include <glib.h>

#include "run-tests.h"

int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);

  add_init_tests ();
  add_hello_tests ();

  return g_test_run ();
}
