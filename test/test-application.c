/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define EXPECTED_TWO_WINDOW_ERRMSG "*You should not add more than one application window*"

static void
test_undefined_two_windows (EosApplication *app)
{
  /* Forking a test case from a signal handler is apparently not
  deterministic */

#if 0
  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GtkWidget *win1, *win2;

      win1 = eos_window_new (app);
      win2 = eos_window_new (app);

      /* Destroy the windows so that the application exits */
      gtk_widget_destroy (win1);
      gtk_widget_destroy (win2);

      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_TWO_WINDOW_ERRMSG);
  gdk_flush ();
#endif
}

void
add_application_tests (void)
{
  /* Tests for undefined behavior, i.e. programming errors */
  if (g_test_undefined ())
    ADD_APP_WINDOW_TEST ("/application/two-windows",
                         test_undefined_two_windows);
}