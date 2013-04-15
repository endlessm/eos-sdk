/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define TEST_APPLICATION_ID "com.endlessm.example.test"
#define EXPECTED_TWO_WINDOW_ERRMSG "*You should not add more than one application window*"

static void
_two_windows_on_startup (EosApplication *app, gpointer data)
{
  GtkWidget *win1, *win2;
  win1 = eos_window_new (app);
  win2 = eos_window_new (app);

  /* Destroy the windows so that the application exits */
  gtk_widget_destroy (win1);
  gtk_widget_destroy (win2);
}

static void
test_undefined_two_windows (void)
{
  EosApplication *app = eos_application_new(TEST_APPLICATION_ID, 0);
  g_signal_connect (app, "startup",
                    G_CALLBACK (_two_windows_on_startup), NULL);

  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      g_application_run (G_APPLICATION (app), 0, NULL);
      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_TWO_WINDOW_ERRMSG);
}

void
add_application_tests (void)
{
  /* Tests for undefined behavior, i.e. programming errors */
  if (g_test_undefined ())
    g_test_add_func ("/application/two-windows", test_undefined_two_windows);
}
