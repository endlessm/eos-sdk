/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define EXPECTED_TWO_WINDOW_ERRMSG "*You should not add more than one application window*"

static void
test_two_windows (EosApplication *app)
{
  GtkWidget *win1 = eos_window_new (app);

  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GtkWidget *win2 = eos_window_new (app);
      gtk_widget_destroy (win2);
      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_TWO_WINDOW_ERRMSG);

  gtk_widget_destroy (win1);
}

void
add_application_tests (void)
{
  ADD_APP_WINDOW_TEST ("/application/two-windows", test_two_windows);
}
