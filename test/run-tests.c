/* Copyright 2013 Endless Mobile, Inc. */

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "run-tests.h"

int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);
  gtk_init (&argc, &argv);

  add_init_tests ();
  add_hello_tests ();
  add_application_tests ();
  add_window_tests ();

  return g_test_run ();
}
