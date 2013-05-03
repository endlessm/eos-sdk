/* Copyright 2013 Endless Mobile, Inc. */

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

/* Test fixture for running a test from an EosApplication's "startup" handler */
void
app_window_test_fixture_setup (AppWindowTestFixture *fixture,
                               gconstpointer callback)
{
  fixture->app = eos_application_new (TEST_APPLICATION_ID, 0);
  g_signal_connect(fixture->app, "startup", G_CALLBACK (callback),
                   NULL);
}

void
app_window_test_fixture_test (AppWindowTestFixture *fixture,
                              gconstpointer unused)
{
  g_application_run (G_APPLICATION (fixture->app), 0, NULL);
}

void
app_window_test_fixture_teardown (AppWindowTestFixture *fixture,
                                  gconstpointer unused)
{
  g_object_unref (fixture->app);
}

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
  add_page_manager_tests ();

  return g_test_run ();
}
