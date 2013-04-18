/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

static void
test_assign_application (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  g_assert(EOS_APPLICATION (app)
           == EOS_APPLICATION (gtk_window_get_application (GTK_WINDOW (win))));

  gtk_widget_destroy (win);
}

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

void
add_window_tests (void)
{
  ADD_APP_WINDOW_TEST ("/window/assign-application", test_assign_application);
  ADD_APP_WINDOW_TEST ("/window/screen-size", test_screen_size);
}
