/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>
#include "endless/eostopbar.h"

#include "run-tests.h"

#define EXPECTED_TOP_BAR_HEIGHT 32
#define EXPECTED_NULL_APPLICATION_ERRMSG \
  "*In order to create a window, you must have an application for it to " \
  "connect to.*"

static void
test_assign_application (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  g_assert(EOS_APPLICATION (app)
           == EOS_APPLICATION (gtk_window_get_application (GTK_WINDOW (win))));

  gtk_widget_destroy (win);
}

static void
test_application_not_null (GApplication *app)
{
  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GtkWidget *win = eos_window_new (NULL);
      gtk_widget_destroy (win);
      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_NULL_APPLICATION_ERRMSG);

  g_application_quit (app); /* No window, so otherwise won't quit */
}

static void
test_screen_size (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  GdkRectangle screen_size, window_size;
  GdkScreen *default_screen = gdk_screen_get_default ();
  gint monitor = 0;

  /* If more than one monitor, find out which one to use */
  if (gdk_screen_get_n_monitors (default_screen) != 1)
    {
      GdkWindow *gdkwindow;

      /* Realize the window so that its GdkWindow is not NULL */
      gtk_widget_realize (GTK_WIDGET (win));
      gdkwindow = gtk_widget_get_window (GTK_WIDGET (win));
      monitor = gdk_screen_get_monitor_at_window (default_screen, gdkwindow);
    }

  gdk_screen_get_monitor_workarea (default_screen, monitor, &screen_size);

  gtk_widget_show_now (GTK_WIDGET (win));

  while (gtk_events_pending ())
    gtk_main_iteration ();

  gtk_widget_get_allocation (GTK_WIDGET (win), &window_size);

  g_assert_cmpint (screen_size.width, ==, window_size.width);
  g_assert_cmpint (screen_size.height, ==, window_size.height);

  gtk_widget_destroy (win);
}

/* Query all the children of win, including the internal children, to find the
top bar */
static void
find_top_bar (GtkWidget *widget,
              GtkWidget **top_bar_return_location)
{
  if (EOS_IS_TOP_BAR (widget))
    *top_bar_return_location = widget;
}

static void
test_has_top_bar (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  GtkWidget *top_bar = NULL;

  gtk_container_forall (GTK_CONTAINER (win), (GtkCallback)find_top_bar,
                        &top_bar);
  g_assert (top_bar != NULL);
  g_assert (EOS_IS_TOP_BAR (top_bar));

  gtk_widget_destroy (win);
}

void
add_window_tests (void)
{
  ADD_APP_WINDOW_TEST ("/window/assign-application", test_assign_application);
  ADD_APP_WINDOW_TEST ("/window/application-not-null",
                       test_application_not_null);
  ADD_APP_WINDOW_TEST ("/window/screen-size", test_screen_size);
  ADD_APP_WINDOW_TEST ("/window/has-top-bar", test_has_top_bar);
}
